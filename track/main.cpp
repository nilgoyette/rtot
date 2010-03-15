#include <iostream>
#include <windows.h>
#include <boost/thread.hpp>

#include "Grabber.h"
#include "DebugGrabber.hpp"
#include "TripleBuffering.h"
#include "ColorThreshold.h"
#include "Tracker.h"

#ifndef _EiC
	#include "cv.h"
	#include "highgui.h"
#endif

using namespace std;
ColorThreshold *colorThreshold;
bool show_hist = 1;

void on_mouse_event( int event, int x, int y, int flags, void* param ){
    colorThreshold->on_mouse(event, x, y, flags,param);
}

bool checkKeys(Tracker &t, ColorThreshold* ct){
    bool retval = true;
    int c = cvWaitKey(10);
    switch( (char) c )
    {
    case 27:
        retval = false;
        break;
    case 'b':
        t.backproject_mode_ ^= 1;
        break;
    case 'c':
        ct->turnOffTracking();
        break;
    default:
    ;
    }
    return retval;
}

void printMenu(){
    printf( "Hot keys: \n"
        "\tESC - quit the program\n"
        "\tc - stop the tracking\n"
        "\tb - switch to/from backprojection view\n"
        "To initialize tracking, select the object with mouse\n" );
}

int main(int argc, char** argv) {
	cvNamedWindow("CamShiftDemo", 1);
	cvNamedWindow("backProject", 1);

    CvSize resolution = cvSize(320, 240);
	TripleBuffering threadBuffer(resolution,false);
	TripleBuffering threadBuffer2(resolution,true);
    DebugGrabber grabber(0, resolution, threadBuffer);
	Tracker track(resolution);
	colorThreshold = new ColorThreshold(resolution,threadBuffer2,track);
	boost::thread threadGrabber(&Grabber::operator(), &grabber);
	boost::thread threadColorThreshold(&ColorThreshold::operator(), colorThreshold);

    cvSetMouseCallback( "CamShiftDemo", on_mouse_event, 0);
    printMenu();
    IplImage* original = NULL;
	IplImage* renderFrame = cvCreateImage(resolution, 8, 3);
    bool loop = true;
	Timer t;
	t.start();
	unsigned int framecount = 0;
    while(loop) {
        // this section will probably change in the future.
		IplImage* tmp = threadBuffer.read();
		if (tmp != original){
            threadBuffer2.write(tmp);          
			original = tmp;
		}
		cvCopyImage(tmp,renderFrame);
		if( colorThreshold->select_object_ && colorThreshold->selection_.width > 0 && colorThreshold->selection_.height > 0 )
		{
			cvSetImageROI( renderFrame,colorThreshold->selection_ );
			cvXorS( renderFrame, cvScalarAll(255), renderFrame, 0 );
			cvResetImageROI( renderFrame );
		}
        if( track.backproject_mode_ && colorThreshold->track_object_ ){
			Circle filtered = track.getNext();		
            if(int(filtered.radius_) > 0 && int(filtered.radius_) <  32768){
                cvCircle(renderFrame,filtered.getCenter(),int(filtered.radius_),CV_RGB(255,0,0),2);
                cvCircle(renderFrame,filtered.getCenter(), 1,CV_RGB(255,0,0), -1, CV_AA, 0);
            }
			
        }
        cvShowImage("CamShiftDemo", renderFrame);
        loop = checkKeys(track, colorThreshold);
		if (++framecount == 60) {
			double d = t.elapsed();
			std::cout << "Display: " << 1 / ((d / 1000) / framecount ) << "fps" << std::endl;
			framecount = 0;
		}
    }
	cvReleaseImage(&renderFrame);
    // this guarenties that the thread exits without errors
    grabber.exit();
    threadGrabber.join();
    colorThreshold->exit();
    threadColorThreshold.join();
    delete colorThreshold;
    return 0;
}

