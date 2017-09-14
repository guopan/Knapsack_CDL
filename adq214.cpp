#include "ADQ214.h"
#include <QMessageBox>

const unsigned int adq_num = 1;

ADQ214::ADQ214(QObject *parent) : QObject(parent)
{
    adq_cu = CreateADQControlUnit();
    success = true;

    int apirev = ADQAPI_GetRevision();     //2?  获取API版本
    qDebug() << IS_VALID_DLL_REVISION(apirev);
    connectADQDevice();

    num_of_devices = 0;
    num_of_failed = 0;
    num_of_ADQ214 = 0;

    setupadq.num_samples_collect = 2048;  //设置采样点数
    setupadq.stream_ch = ADQ214_STREAM_ENABLED_BOTH;
    setupadq.stream_ch &= 0x7;
    setupadq.num_buffers = 64;
    setupadq.size_buffers = 1024;
    setupadq.clock_source = 0;       //0 = Internal clock
    setupadq.pll_divider = 2;        //在Internal clock=0时，设置，f_clk = 800MHz/divider
}

void ADQ214::connectADQDevice()
{
    int num_of_devices,num_of_failed,num_of_ADQ214;
    num_of_devices = ADQControlUnit_FindDevices(adq_cu);			//找到所有与电脑连接的ADQ，并创建一个指针列表，返回找到设备的总数
    num_of_failed = ADQControlUnit_GetFailedDeviceCount(adq_cu);
    num_of_ADQ214 = ADQControlUnit_NofADQ214(adq_cu);				//返回找到ADQ214设备的数量
    if((num_of_failed > 0)||(num_of_devices == 0))
    {
        qDebug()<<"采集卡未连接";
        isADQ214Connected = false;
        //        QMessageBox::critical(this, QString::("采集卡未连接！！"), QString::fromStdString("采集卡未连接"));
    }
    else if (num_of_ADQ214 != 0)
    {
        qDebug()<<"采集卡已连接";
        isADQ214Connected = true;
        // beginADQ();
    }
}

void ADQ214::Start_Capture()
{
    //    int num_buffers = 8;
    //    int size_buffers = 1024;
    //    success = success && ADQ214_SetTransferBuffers(adq_cu, adq_num, num_buffers, size_buffers);

    if(!Config_ADQ214())
        return;


    setupadq.data_stream_target = new qint16[setupadq.num_samples_collect];
    memset(setupadq.data_stream_target, 0, setupadq.num_samples_collect);

    if(!CaptureData2Buffer())
        return;

    WriteData2disk();
    WriteSpecData2disk();
    //    Display_Data();

    qDebug() << ("Collect finished!");
    delete setupadq.data_stream_target;
    if(success == 0)
    {
        qDebug() << "Error!";
        DeleteADQControlUnit(adq_cu);
    }

}

bool ADQ214::Config_ADQ214()                   // 配置采集卡
{
    success = false;
    if (!isADQ214Connected)
    {
        QMessageBox::critical(NULL, QString::fromStdString("采集卡未连接！！"), QString::fromStdString("采集卡未连接"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        ADQ214_SetDataFormat(adq_cu, adq_num,ADQ214_DATA_FORMAT_UNPACKED_14BIT);
        // 设置TransferBuffer大小及数量
        //    success = ADQ214_SetTransferBuffers(adq_cu, adq_num, setupadq.num_buffers, setupadq.size_buffers);


        success = ADQ214_SetClockSource(adq_cu, adq_num, setupadq.clock_source);

        success = success && ADQ214_SetPllFreqDivider(adq_cu, adq_num, setupadq.pll_divider);
    }
    return success;
}

bool ADQ214::CaptureData2Buffer()                   // 采集数据到缓存
{
    success = ADQ214_DisarmTrigger(adq_cu, adq_num);
    success = success && ADQ214_SetStreamStatus(adq_cu, adq_num,setupadq.stream_ch);
    success = success && ADQ214_ArmTrigger(adq_cu, adq_num);

    unsigned int samples_to_collect;
    samples_to_collect = setupadq.num_samples_collect;

    int nloops = 0;

    ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,setupadq.spi_data_out[0]&0xFFFE);   // bit[0]置0
    ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,setupadq.spi_data_out[0]|0x0001);   // bit[0]置1

    while (samples_to_collect > 0)
    {
        nloops ++;
        qDebug() << "Loops:" << nloops;
        do
        {
            setupadq.collect_result = ADQ214_GetTransferBufferStatus(adq_cu, adq_num, &setupadq.buffers_filled);
            qDebug() << ("Filled: ") << setupadq.buffers_filled;
        } while ((setupadq.buffers_filled == 0) && (setupadq.collect_result));

        setupadq.collect_result = ADQ214_CollectDataNextPage(adq_cu, adq_num);

        int samples_in_buffer = qMin(ADQ214_GetSamplesPerPage(adq_cu, adq_num), samples_to_collect);
        qDebug() << "samples_in_buffer = " << samples_in_buffer;

        if (ADQ214_GetStreamOverflow(adq_cu, adq_num))
        {
            qDebug() << ("Warning: Streaming Overflow!");
            setupadq.collect_result = 0;
        }

        if (setupadq.collect_result)
        {
            // Buffer all data in RAM before writing to disk, if streaming to disk is need a high performance
            // procedure could be implemented here.
            // Data format is set to 16 bits, so buffer size is Samples*2 bytes
            memcpy((void*)&setupadq.data_stream_target[setupadq.num_samples_collect - samples_to_collect],
                    ADQ214_GetPtrStream(adq_cu, adq_num), samples_in_buffer* sizeof(signed short));
            samples_to_collect -= samples_in_buffer;
            qDebug() << " AA= "<<samples_to_collect;
        }
        else
        {
            qDebug() << ("Collect next data page failed!");
            samples_to_collect = 0;
        }
    }

    success = success && ADQ_DisarmTrigger(adq_cu, adq_num);
    success = success && ADQ214_SetStreamStatus(adq_cu, adq_num,0);
    return success;
}

void ADQ214::WriteData2disk()                   // 将数据直接写入文件
{
    // Write to data to file after streaming to RAM, because ASCII output is too slow for realtime.
    qDebug() << "Writing stream data in RAM to disk" ;

    setupadq.stream_ch &= 0x07;
    QFile fileA("dataA.txt");
    QFile fileB("dataB.txt");


    QTextStream out(&fileA);
    QTextStream out2(&fileB);

    unsigned int samples_to_collect = setupadq.num_samples_collect;
    if(fileA.open(QFile::WriteOnly)&&fileB.open(QFile::WriteOnly))
    {
        while (samples_to_collect > 0)
        {
            for (int i=0; (i<4) && (samples_to_collect>0); i++)
            {
                out << setupadq.data_stream_target[setupadq.num_samples_collect-samples_to_collect] << endl;
                qDebug()<<"CHA -- "<<setupadq.num_samples_collect-samples_to_collect;
                samples_to_collect--;
            }

            for (int i=0; (i<4) && (samples_to_collect>0); i++)
            {
                out2 << setupadq.data_stream_target[setupadq.num_samples_collect-samples_to_collect] << endl;
                qDebug()<<"CHB -- "<<setupadq.num_samples_collect-samples_to_collect;
                samples_to_collect--;
            }
        }
    }
    fileA.close();
    fileB.close();
}

void ADQ214::WriteSpecData2disk()                   // 将数据转换成功率谱，写入到文件
{
    // Write to data to file after streaming to RAM, because ASCII output is too slow for realtime.

    if(setupadq.num_samples_collect%2048 != 0)
        return;
    int nLoops = setupadq.num_samples_collect/2048;


    setupadq.stream_ch &= 0x07;

    if (psd_res != nullptr)
        delete psd_res;
    int psd_datanum = 512*nLoops;        //功率谱长度
    psd_res = new PSD_DATA[psd_datanum];

    int i = 0, k = 0, l = 0;
    for (l=0;l<nLoops;l++)
        for (k=0,i=0; (k<512); k++,k++)
        {
            psd_res[512*l + BitReverseIndex[k]].pos[3] = setupadq.data_stream_target[2048*l + i];
            psd_res[512*l + BitReverseIndex[k]].pos[2] = setupadq.data_stream_target[2048*l + i+1];
            psd_res[512*l + BitReverseIndex[k]].pos[1] = setupadq.data_stream_target[2048*l + i+4];
            psd_res[512*l + BitReverseIndex[k]].pos[0] = setupadq.data_stream_target[2048*l + i+5];
            psd_res[512*l + BitReverseIndex[k+1]].pos[3] = setupadq.data_stream_target[2048*l + i+2];
            psd_res[512*l + BitReverseIndex[k+1]].pos[2] = setupadq.data_stream_target[2048*l + i+3];
            psd_res[512*l + BitReverseIndex[k+1]].pos[1] = setupadq.data_stream_target[2048*l + i+6];
            psd_res[512*l + BitReverseIndex[k+1]].pos[0] = setupadq.data_stream_target[2048*l + i+7];

            i = i + 8;
            qDebug()<<"Union.Spec["<<BitReverseIndex[k]<<"] = "<<psd_res[512*l + BitReverseIndex[k]].data64;
            qDebug()<<"Union.Spec["<<BitReverseIndex[k+1]<<"] = "<<psd_res[512*l + BitReverseIndex[k+1]].data64;
        }
    qDebug() << "Writing streamed Spectrum data in RAM to disk" ;
    QFile Specfile("data_Spec.txt");
    if(Specfile.open(QFile::WriteOnly))
    {
        QTextStream out(&Specfile);
        for (k=0; (k<psd_datanum); k++)
            out <<psd_res[k].data64 << endl;
    }
    Specfile.close();
}
