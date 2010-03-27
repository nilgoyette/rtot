
#include "Grabber.h"

#include <iostream>

Grabber::Grabber(const int camId, const CvSize resolution, TripleBuffering& buffer1,TripleBuffering& buffer2) throw()
		: camId_(camId),
		  capture_(NULL),
		  sink1_(buffer1),
		  sink2_(buffer2),
		  initialized_(false),
          exit_(true) {
	capture_ = cvCaptureFromCAM(camId_);
	const double fps = 30.0;
	cvSetCaptureProperty(capture_, CV_CAP_PROP_FRAME_WIDTH, resolution.width);
	cvSetCaptureProperty(capture_, CV_CAP_PROP_FRAME_HEIGHT, resolution.height);
	cvSetCaptureProperty(capture_, CV_CAP_PROP_FPS, fps);

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
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	if (!initialized_) {
		printf("Grabber was not initialised\n");
		return;
	}

	// DO NOT RELEASE THIS IMAGE  
	IplImage *frame;
	while (exit_) {
        beforeGrab();
		frame = cvQueryFrame(capture_); 
		if (!frame) {
			return;
		}
		sink1_.write(frame);
		sink2_.write(frame);
	}
}

void Grabber::beforeGrab() throw() {}
