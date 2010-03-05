
#include <iostream>
#include <windows.h>
#include <boost/thread.hpp>

//#include "Grabber.h"
#include "DebugGrabber.hpp"
#include "Timer.hpp"
#include "TripleBuffering.h"
#include "ColorThreshold.h"

#ifndef _EiC
	#include "cv.h"
	#include "highgui.h"
#endif

using namespace std;
ColorThreshold *cth;

void on_mouse_event( int event, int x, int y, int flags, void* param ){
    cth->hist_.on_mouse(event, x, y, flags,param);
}

int main(int argc, char** argv) {
	//CvSize resolution = cvSize(320, 240);
    CvSize resolution = cvSize(160, 120);
	TripleBuffering threadBuffer(resolution);
	DebugGrabber g(0, resolution, threadBuffer);

	boost::thread tG(&Grabber::operator (), &g);
	cvNamedWindow("CamShiftDemo", 1);
    cth = new ColorThreshold(resolution);
    cvSetMouseCallback( "CamShiftDemo", on_mouse_event, 0);

	for(;;) {
		int c = cvWaitKey(10);
		if (c == 27) {
			break;
		}
		cvShowImage("CamShiftDemo", cth->process( threadBuffer.read()));
	}

	return 0;
}
