
#include "Grabber.h"

Grabber::Grabber(const int camId, const CvSize resolution, TripleBuffering& buffer) throw()
		: camId_(camId),
		  capture_(NULL),
		  sink_(buffer),
		  initialized_(false) {
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

void Grabber::operator()() throw() {
	if (!initialized_) {
		printf("Grabber was not initialised\n");
		return;
	}

	// DO NOT RELEASE THIS IMAGE  
	IplImage *frame;
	for (;;) {
        beforeGrab();
		frame = cvQueryFrame(capture_); 
		sink_.write(frame);
	}
}

void Grabber::beforeGrab() throw() {}
