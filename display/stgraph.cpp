#include "stgraph.h"

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap( Qt::white, Qt::darkRed )
    {
        addColorStop( 0,    Qt::white);
        addColorStop( 0.01, Qt::blue );
        addColorStop( 0.4,  Qt::cyan );
        addColorStop( 0.6,  Qt::yellow );
        addColorStop( 0.8,  Qt::red );
    }
};

STGraph::STGraph( QWidget *parent ):
    QwtPlot( parent )
{
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );                         //»­²¼ËÄÖÜ±ß¿ò½Ç°ë¾¶
    setCanvas( canvas );

    d_spectrogram = new QwtPlotSpectrogram();

    setAxisScale( QwtPlot::xBottom, -10, 0 );
    setAxisMaxMinor( QwtPlot::xBottom, 0 );
//    ColorMap *myColorMap = new ColorMap();
    d_spectrogram->setColorMap( new ColorMap());

    QFont font("Î¢ÈíÑÅºÚ",12);
    font.setBold(true);
    setAxisFont(QwtPlot::yLeft,font);
    setAxisFont(QwtPlot::xBottom,font);

    stData = new STData;
}

void STGraph::initialShow(int nLayers, double minHeight, double heightStep)
{
    stData->setSize(nLayers);
    stData->setdata(new double[nLayers]{0});
    stData->setHeightsLabel(minHeight, minHeight + (nLayers-1)*heightStep);

    d_spectrogram->setData( stData );
    d_spectrogram->attach(this);
    setAxisScale( QwtPlot::yLeft, minHeight, minHeight + (nLayers-1)*heightStep, heightStep );

//    static_cast<STData *>( d_spectrogram->data() )->setResampleMode( static_cast<QwtMatrixRasterData::ResampleMode>( 0 ));
    stData->setResampleMode(QwtMatrixRasterData::NearestNeighbour);

    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, false);       //ÉèÖÃÏÔÊ¾Ä£Ê½
    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, true);

    //zÖá
    zInterval.setInterval(0,15);
    // A color bar on the right axis
//    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorBarWidth( 40 );
    rightAxis->setColorMap( zInterval, new ColorMap());
    QFont font("Î¢ÈíÑÅºÚ",12);
    font.setBold(true);
    rightAxis->setFont(font);
    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    enableAxis( QwtPlot::yRight );
}

void STGraph::updateShow(const double *data_in)
{
    stData->setdata(data_in);
    d_spectrogram->setData( stData );
    d_spectrogram->attach(this);

    zInterval = d_spectrogram->data()->interval( Qt::ZAxis );
    rightAxis->setColorMap( zInterval, new ColorMap() );
    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );

    replot();

}

