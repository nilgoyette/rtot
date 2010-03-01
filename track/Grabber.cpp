#include "Grabber.h"

Grabber::Grabber(int camId, int width, int height) 
		: camId_(camId), width_(width),height_(height)
{
	capture = 0;
	capture = cvCaptureFromCAM(camId_);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width_);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height_);
	frame = 0;
	image = 0;

	imgPointers.backproject = 0;
	imgPointers.c_h = 0;
	imgPointers.c_s = 0;
	imgPointers.c_v = 0;
	imgPointers.hsv = 0;
	imgPointers.locked = false;

	histimg = 0;
	hist = 0;
	hranges_arr[0] = 0;
	hranges_arr[1] = 180;
	hranges = hranges_arr;
	hdims = 16;
}

Grabber::~Grabber(void)
{
}

void Grabber::grabber(void){
	for(;;){
		frame = cvQueryFrame( capture ); //DO NOT RELEASE THIS IMAGE
		if (!frame)
			break;
		
		if (!image) {
			boost::lock_guard<boost::mutex> lock(mut);
			imgPointers.locked = true;
			/* allocate all the buffers */
			image = cvCreateImage( cvGetSize(frame), 8, 3 );

			image->origin = frame->origin;
			imgPointers.hsv = cvCreateImage( cvGetSize(frame), 8, 3 );
			imgPointers.c_h = cvCreateImage( cvGetSize(frame), 8, 1 );
			imgPointers.c_s = cvCreateImage( cvGetSize(frame), 8, 1 );
			imgPointers.c_v = cvCreateImage( cvGetSize(frame), 8, 1 );
			
			imgPointers.backproject = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
			cvCvtColor( frame, imgPointers.hsv, CV_BGR2Lab );


			// related to histogram
			hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
			histimg = cvCreateImage( cvSize(320,200), 8, 3 );
		}
		
	}
}

void threadStart(void){

}
