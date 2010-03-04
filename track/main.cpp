
#include <iostream>
#include <windows.h>
#include <boost/thread.hpp>

#include "Grabber.h"
#include "Timer.h"
#include "TripleBuffering.h"

#ifndef _EiC
	#include "cv.h"
	#include "highgui.h"
#endif

using namespace std;

int main(int argc, char** argv) {
	CvSize resolution = cvSize(320, 240);
	TripleBuffering threadBuffer(resolution);
	Grabber g(0, resolution, threadBuffer);


    Timer t;
	boost::thread tG(&Grabber::operator (), &g);
	cvNamedWindow("CamShiftDemo", 1);
	for(;;) {
		int c = cvWaitKey(10);
		if (c == 27) {
			break;
		}
        t.start();
		cvShowImage("CamShiftDemo", threadBuffer.read());
		double d = t.elapsed();
		t.reset();
		cout << d << endl;
	}

	return 0;
}
