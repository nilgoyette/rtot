
#include "Grabber.h"
#include <iostream>

Grabber::Grabber(const int camId, const CvSize resolution, TripleBuffering& buffer) throw()
		: camId_(camId),
		  capture_(NULL),
		  sink_(buffer),
		  initialized_(false),
          exit_(true){
	capture_ = cvCaptureFromCAM(camId_);
	cvSetCaptureProperty(capture_, CV_CAP_PROP_FRAME_WIDTH, resolution.width);
	cvSetCaptureProperty(capture_, CV_CAP_PROP_FRAME_HEIGHT, resolution.height);

	if (!capture_) {
		printf("Could not initialize capturing...\n");
		return;
	}
	initialized_ = true;

	

}

Grabber::~Grabber(void) throw() {
	if (capture_ != NULL) {
		cvReleaseCapture(&capture_);
	}
}

void Grabber::exit(){
    exit_ = false;
}

void Grabber::operator()() throw() {
	if (!initialized_) {
		printf("Grabber was not initialised\n");
		return;
	}

	// DO NOT RELEASE THIS IMAGE  
	IplImage *frame;
	while (exit_) {
        beforeGrab();
		frame = cvQueryFrame(capture_); 
		if(!frame)
			return;
		sink_.write(frame);
	}
}

void Grabber::beforeGrab() throw() {}
