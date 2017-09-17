#include <losvelocity.h>

using namespace Eigen;
using namespace std;

LOSVelocity::LOSVelocity(double da[], int heightNum, int spectNumPerRan, int losVelocityBandPoints, double wavelength, double freqResol)
{
    data = MatrixXd::Zero(heightNum,spectNumPerRan);
    radialvelo =  MatrixXd::Zero(heightNum-1,1);
    FreqAxis = MatrixXd::Zero(1,spectNumPerRan);
    //Ƶ����FreqAxis
    for(int i=0;i<spectNumPerRan;i++)
        {
            FreqAxis(0,i) = freqResol*i;
        }

    //���ݰ�heightNum*spectNumPerRanת��Ϊ��ά����
    for(int i=0;i<heightNum;i++) {
        for (int j=0; j<spectNumPerRan; j++)
        {
            data(i,j) = da[i*spectNumPerRan+j];
        }
    }
//    std::cout << data << std::endl;

    //��ӽ�AOM��Ƶ��
    double min = abs(FreqAxis(0,0) - AOM);
    int Index;                //��ӽ�AOMƵ���±�
    int FreqDw;               //Ƶ���±�
    int FreqUp;               //Ƶ���ϱ�
    for(int m = 0; m<spectNumPerRan; m++) {
        if(min > abs(FreqAxis(0,m) - AOM)) {
            min = abs(FreqAxis(0,m) - AOM);
            Index = m;
        }
    }

    FreqDw = Index-losVelocityBandPoints;
    FreqUp = Index+losVelocityBandPoints;
    std::cout << "Index = \n" << Index << std::endl;

   DopplerFreq = MatrixXd::Zero(heightNum-1,1);          //DopplerFreq =43*1
    //�����׼�ȥ���һ���������ݲ��ҳ����ֵ
    for(int row = 0; row<heightNum-1; row++)
    {
        int k = 0;
        double max = data(row,FreqDw) - data(43,FreqDw);
        for (int col=FreqDw; col<FreqUp; col++) {
            if (max<(data(row,col) - data(43,col))) {
                max = data(row,col) - data(43,col);
                k = col;
            }
        }
        DopplerFreq(row,0) = FreqAxis(0,k);
    }
    std::cout << "end!" << endl;
    //�����ٶȼ���
    for(int row=0;row<heightNum-1;row++) {
        radialvelo(row,0) = -(DopplerFreq(row,0) - AOM)*wavelength/2;
    }

}


