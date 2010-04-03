
#include "ColorThreshold.h"
#include <iostream>

#include "cv.h"
#include "highgui.h"

ColorThreshold::ColorThreshold(CvSize size,TripleBuffering& source,Tracker& track) 
    : exit_(true),
      source_(source),
	  track_(track),
	  hsv_(cvCreateImage(size, 8, 3)),
      mask_(cvCreateImage(size, 8, 1)),
	  backproject_(cvCreateImage(size, 8, 1)),
	  threshold_(100), 
      se21_(cvCreateStructuringElementEx((6 * 2) + 1, (6 * 2) + 1, 6, 6, CV_SHAPE_ELLIPSE, NULL)),
      se11_(cvCreateStructuringElementEx((1 * 2) + 1, (1 * 2) + 1, 1, 1, CV_SHAPE_ELLIPSE, NULL)),
	  seHistogram1_(cvCreateStructuringElementEx((10 * 2) + 1, (10 * 2) + 1, 10, 10, CV_SHAPE_RECT, NULL)),
	  seHistogram2_(cvCreateStructuringElementEx((4 * 2) + 1, (4 * 2) + 1, 4, 4, CV_SHAPE_RECT, NULL)),
	  calcHist_(false),
	  size_(size),
	  hist_(Histogram(size)),
	  track_object_(false),
	  calcule_hist_(false){}

void ColorThreshold::exit() {
    exit_ = false;
}

ColorThreshold::~ColorThreshold(void) {
    cvReleaseImage(&backproject_);
	cvReleaseImage(&hsv_);
	cvReleaseImage(&mask_);
    cvReleaseStructuringElement(&se21_);
    cvReleaseStructuringElement(&se11_);
	cvReleaseStructuringElement(&seHistogram1_);
	cvReleaseStructuringElement(&seHistogram2_);
	
	
}

void ColorThreshold::on_mouse(int event, int x, int y, int flags, void* param) {
}

void ColorThreshold::operator()() throw() {
	while (exit_) {
		IplImage* frame = source_.read();
		IplImage* cthFrame = process(frame);
		track_.process(cthFrame);
		cvShowImage("backProject", cthFrame);
	}
}

IplImage* ColorThreshold::process(IplImage* frame) { 

	////segment blue without histogram
	//cvCvtColor(frame, lab_, CV_BGR2Lab);
	//cvSplit(lab_, 0, 0, backproject_, 0);
	//cvThreshold(backproject_, backproject_, 100, 255, CV_THRESH_BINARY);
	//cvXorS(backproject_, cvScalarAll(255), backproject_, 0);
	//cvMorphologyEx(backproject_, backproject_, NULL, se21_, CV_MOP_CLOSE); // See completed example for cvClose definition	
 //   track_object_ = true;
	//return backproject_;
	

	if (track_object_ || calcule_hist_) {
        // Calcule image backproject using histogram
		if (calcule_hist_) {
            hist_.createHistogram(frame, selection_);
			CvArr* src = hist_.hist_->bins;
			cvMorphologyEx(src,src, NULL, seHistogram1_, CV_MOP_CLOSE);
			cvDilate(src,src,seHistogram2_);
			track_object_ = true;
			calcule_hist_ = false;
		}
		cvCvtColor(frame, hsv_, CV_BGR2HSV);
		cvInRangeS(hsv_, cvScalar(0,0,COLOR_VMIN,0),cvScalar(180,256,COLOR_VMAX,0), mask_);
		cvSmooth(frame, frame, CV_GAUSSIAN, 5, 5, 0, 0);
        hist_.getBackProject(frame, backproject_);	
		cvAnd(backproject_, mask_, backproject_, 0);
		cvThreshold(backproject_, backproject_, 100, 255, CV_THRESH_BINARY);
		cvMorphologyEx(backproject_, backproject_, NULL, se21_, CV_MOP_CLOSE);
		return backproject_;
    }
    
    return frame;
}

void ColorThreshold::turnOffTracking(){
	track_object_ = false;
	initialized_ = false;
	cvDestroyWindow("H-S Histogram");
}
