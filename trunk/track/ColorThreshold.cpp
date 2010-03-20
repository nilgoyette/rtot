
#include "ColorThreshold.h"
#include <iostream>

#include "cv.h"
#include "highgui.h"

ColorThreshold::ColorThreshold(CvSize size,TripleBuffering& source,Tracker& track) 
    : exit_(true),
      source_(source),
	  track_(track),
      lab_(cvCreateImage(size, 8, 3)),
	  backproject_(cvCreateImage(size, 8, 1)),
	  threshold_(100), 
      se21_(cvCreateStructuringElementEx((6 * 2) + 1, (6 * 2) + 1, 6, 6, CV_SHAPE_RECT, NULL)),
      se11_(cvCreateStructuringElementEx((3 * 2) + 1, (3 * 2) + 1, 3, 3, CV_SHAPE_RECT, NULL)),
	  setest_(cvCreateStructuringElementEx((10 * 2) + 1, (10 * 2) + 1, 10, 10, CV_SHAPE_RECT, NULL)),
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
    cvReleaseStructuringElement(&se21_);
    cvReleaseStructuringElement(&se11_);
	cvReleaseStructuringElement(&setest_);
	
}

void ColorThreshold::on_mouse(int event, int x, int y, int flags, void* param) {
}

void ColorThreshold::operator()() throw() {
	while (exit_) {
		IplImage* frame = source_.read();
		//IplImage* cthFrame = bgSeg.process(frame);
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
			cvMorphologyEx(src,src, NULL, setest_, CV_MOP_CLOSE);
			cvDilate(src,src,setest_);
			
			hist_.show();
			track_object_ = true;
			calcule_hist_ = false;
		}
        hist_.getBackProject(frame, backproject_);
		int dia = 300 / 20 + 1;
		cvThreshold(backproject_, backproject_, 20, 255, CV_THRESH_BINARY);
		cvMorphologyEx(backproject_, backproject_, NULL, se11_, CV_MOP_OPEN );  // See completed example for cvOpen  definition
		cvMorphologyEx(backproject_, backproject_, NULL, se21_, CV_MOP_CLOSE); // See completed example for cvClose definition	
		return backproject_;
    }
    //frame = hist_.aplyRoiToImage(frame,selection_);
	
    return frame;
}

void ColorThreshold::turnOffTracking(){
	track_object_ = false;
	initialized_ = false;
	cvDestroyWindow("H-S Histogram");
}
