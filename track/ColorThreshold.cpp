
#include "ColorThreshold.h"

#include <iostream>

#include "cv.h"
#include "highgui.h"

ColorThreshold::ColorThreshold(CvSize size,TripleBuffering& source,Tracker& track) 
    : exit_(true),
      source_(source),
	  track_(track),
      backproject_(cvCreateImage(size, 8, 1)),
	  threshold_(100), 
      se21_(cvCreateStructuringElementEx((6 * 2) + 1, (6 * 2) + 1, 6, 6, CV_SHAPE_RECT, NULL)),
      se11_(cvCreateStructuringElementEx((3 * 2) + 1, (3 * 2) + 1, 3, 3, CV_SHAPE_RECT, NULL)),
	  setest_(cvCreateStructuringElementEx((10 * 2) + 1, (10 * 2) + 1, 10, 10, CV_SHAPE_RECT, NULL)),
	  calcHist_(false),
	  size_(size),
	  hist_(Histogram(size)),
	  track_object_(false),
	  calcule_hist_(false),
	  select_object_(false) {}

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
	if (select_object_) {
		selection_.x = std::min(x, origin_.x);
		selection_.y = std::min(y, origin_.y);
		selection_.width = selection_.x + abs(x - origin_.x);
		selection_.height = selection_.y + abs(y - origin_.y);
		selection_.x = std::max(selection_.x, 0);
		selection_.y = std::max(selection_.y, 0);
		selection_.width = std::min(selection_.width, size_.width);
		selection_.height = std::min(selection_.height, size_.height);
		selection_.width -= selection_.x;
		selection_.height -= selection_.y;
	}

	switch (event) {
		case CV_EVENT_LBUTTONDOWN:
			origin_ = cvPoint(x, y);
			selection_ = cvRect(x, y, 0, 0);
			select_object_ = true;
			break;
		case CV_EVENT_LBUTTONUP:
			select_object_ = false;
			if (selection_.width > 0 && selection_.height > 0 ) {
				calcule_hist_ = true;
				track_object_ = false;
			}
			break;
	}
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
	if (track_object_ || calcule_hist_) {
        // Calcule image backproject using histogram
		if (calcule_hist_) {
            hist_.createHistogram(frame, selection_);
			CvArr* src = hist_.hist_->bins;
			cvMorphologyEx(src,src, NULL, setest_, CV_MOP_CLOSE);
			cvDilate(src,src,se11_);
			hist_.show();
			track_object_ = true;
			calcule_hist_ = false;
		}
        hist_.getBackProject(frame, backproject_);
		cvSmooth(backproject_, backproject_, CV_GAUSSIAN, 31, 31, 0, 0);
		int dia = 300 / 20 + 1;
		cvThreshold(backproject_, backproject_, 20, 255, CV_THRESH_BINARY);
		//cvMorphologyEx(backproject_, backproject_, NULL, se21_, CV_MOP_CLOSE); // See completed example for cvClose definition	
		//cvMorphologyEx(backproject_, backproject_, NULL, se11_, CV_MOP_OPEN );  // See completed example for cvOpen  definition
       
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
