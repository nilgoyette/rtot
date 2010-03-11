#include "ColorThreshold.h"
#include <iostream>
#include "highgui.h"

ColorThreshold::ColorThreshold(CvSize size) 
    : backproject_(cvCreateImage(size,8,1)), threshold_(100), 
      se21_(cvCreateStructuringElementEx((10*2)+1, (10*2)+1, 10, 10, CV_SHAPE_RECT, NULL)),
      se11_(cvCreateStructuringElementEx((3*2)+1, (3*2)+1, 3,  3,  CV_SHAPE_RECT, NULL)), calcHist_(false),
      size_(size), hist_(Histogram(size)), track_object_(false),calcule_hist_(false),select_object_(false)
{
}

ColorThreshold::~ColorThreshold(void)
{
    cvReleaseImage(&backproject_);
    cvReleaseStructuringElement(&se21_);
    cvReleaseStructuringElement(&se11_);
}

void ColorThreshold::on_mouse( int event, int x, int y, int flags, void* param ){
	if( select_object_ ){
		selection_.x = min(x,origin_.x);
		selection_.y = min(y,origin_.y);
		selection_.width = selection_.x + abs(x - origin_.x);
		selection_.height = selection_.y + abs(y - origin_.y);
		selection_.x = max( selection_.x, 0 );
		selection_.y = max( selection_.y, 0 );
		selection_.width = min( selection_.width, size_.width );
		selection_.height = min( selection_.height, size_.height );
		selection_.width -= selection_.x;
		selection_.height -= selection_.y;
       
	}

	switch( event )
	{
	case CV_EVENT_LBUTTONDOWN:
		origin_ = cvPoint(x,y);
		selection_ = cvRect(x,y,0,0);
		select_object_ = true;
		break;
	case CV_EVENT_LBUTTONUP:
		select_object_ = false;
		if( selection_.width > 0 && selection_.height > 0 ){
			calcule_hist_ = true;
			track_object_ = false;
		}
		break;
	}
}

IplImage* ColorThreshold::process(IplImage* frame){
	
	if( select_object_ && selection_.width > 0 && selection_.height > 0 )
	{
		cvSetImageROI( frame, selection_ );
		cvXorS( frame, cvScalarAll(255), frame, 0 );
		cvResetImageROI( frame );
	}
	 
	if( track_object_ || calcule_hist_){
        //calcule image backproject using histogram
		if (calcule_hist_){
            hist_.createHistogram(frame,selection_);
			track_object_ = true;
			calcule_hist_ = false;
		}
      
        hist_.getBackProject(frame,backproject_);
      
        cvMorphologyEx(backproject_, backproject_, NULL,se21_,CV_MOP_CLOSE); // See completed example for cvClose definition	
        cvMorphologyEx(backproject_, backproject_, NULL, se11_,CV_MOP_OPEN );  // See completed example for cvOpen  definition
        cvSmooth(backproject_, backproject_, CV_GAUSSIAN, 5,5, 0, 0);
        cvThreshold(backproject_,backproject_,threshold_,255,CV_THRESH_BINARY | CV_THRESH_OTSU );
        return backproject_;
    }
    //frame = hist_.aplyRoiToImage(frame,selection_);
	
    return frame;
}

void ColorThreshold::turnOffTracking(){
	track_object_ = false;
	initialized_ = false;
	cvDestroyWindow( "H-S Histogram" );
}