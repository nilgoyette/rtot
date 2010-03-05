#include "ColorThreshold.h"
#include "highgui.h"

ColorThreshold::ColorThreshold(CvSize size) 
    : backproject_(cvCreateImage(size,8,1)), threshold_(100), 
      se21_(cvCreateStructuringElementEx((10*2)+1, (10*2)+1, 10, 10, CV_SHAPE_RECT, NULL)),
      se11_(cvCreateStructuringElementEx((3*2)+1, (3*2)+1, 3,  3,  CV_SHAPE_RECT, NULL)), calcHist_(false),
      hist_(Histogram(size))
{
}

ColorThreshold::~ColorThreshold(void)
{
    cvReleaseStructuringElement(&se21_);
    cvReleaseStructuringElement(&se11_);
}

void ColorThreshold::setHist(){
    calcHist_ = !calcHist_;
}

IplImage* ColorThreshold::process(IplImage* frame){
    if( hist_.track_object_ ){
        //calcule image backproject using histogram
        if( !hist_.initialized_ ){
            hist_.createHistogram(frame);
        }

        cvCalcBackProject( hist_.planes_, backproject_, hist_.process(frame) );

        cvMorphologyEx(backproject_, backproject_, NULL,se21_,CV_MOP_CLOSE); // See completed example for cvClose definition	
        cvMorphologyEx(backproject_, backproject_, NULL, se11_,CV_MOP_OPEN );  // See completed example for cvOpen  definition
        cvSmooth(backproject_, backproject_, CV_GAUSSIAN, 5,5, 0, 0);
        cvThreshold(backproject_,backproject_,threshold_,255,CV_THRESH_BINARY | CV_THRESH_OTSU );
        return backproject_;
    }
    frame = hist_.aplyRoiToImage(frame);
    return frame;
}