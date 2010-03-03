#include <iostream>
#include <boost/thread.hpp>
#include "TripleBuffering.h"
#include "Grabber.h"

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
//#include <stdio.h>
//#include <ctype.h>
//#include "BlobResult.h"
#endif

using namespace std;

int main(int argc, char** argv) {
	CvSize size;
	size.width = 320;
	size.height = 240;
	TripleBuffering threadBuffer(size);
	Grabber g(0,size,threadBuffer);

	boost::thread tG( &Grabber::operator (), &g );
	cvNamedWindow("CamShiftDemo", 1);
	for(;;) {
		int c;
		c = cvWaitKey(10);
		if( (char) c == 27 )
			break;
		IplImage *img = cvCreateImage(cvSize(800, 600), 8, 3);
		cvShowImage("CamShiftDemo", threadBuffer.read());
	}

	return 0;
}
