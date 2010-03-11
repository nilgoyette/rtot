#include <iostream>
#include <windows.h>
#include <boost/thread.hpp>

//#include "Grabber.h"
#include "DebugGrabber.hpp"
#include "Timer.hpp"
#include "TripleBuffering.h"
#include "ColorThreshold.h"
#include "Tracker.h"

#ifndef _EiC
	#include "cv.h"
	#include "highgui.h"
#endif

using namespace std;
ColorThreshold *cth;
bool show_hist = 1;

void on_mouse_event( int event, int x, int y, int flags, void* param ){
    cth->on_mouse(event, x, y, flags,param);
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
    //CvSize resolution = cvSize(320, 240);
    CvSize resolution = cvSize(160, 120);
    TripleBuffering threadBuffer(resolution);
    DebugGrabber g(0, resolution, threadBuffer);

    boost::thread tG(&Grabber::operator (), &g);
    cvNamedWindow("CamShiftDemo", 1);
	cvNamedWindow("backProject", 1);
    cth = new ColorThreshold(resolution);
    cvSetMouseCallback( "CamShiftDemo", on_mouse_event, 0);
    Tracker track(resolution);

    printMenu();
    IplImage* cthFrame;
    IplImage* original;
    bool loop = true;
    while(loop) {
        // this section will probably change in the future.
        original = threadBuffer.read();
        cthFrame = cth->process( original );

        if( track.backproject_mode_ && cth->track_object_ ){
            Circle retCircle = track.process(cthFrame);
            if(int(retCircle.radius_) > 0 && retCircle.radius_ <  32768){
                cvCircle(original,retCircle.center_,retCircle.radius_,CV_RGB(255,0,0),3);
                cvCircle(original,retCircle.center_, 3,CV_RGB(255,0,0), -1, CV_AA, 0);
            }
			int roisize  = track.current.radius_ * 2.5 ;
			int x = std::max(0,track.current.center_.x - roisize);
			int y = std::max(0,track.current.center_.y - roisize);
			x = std::min(cthFrame->width-1,x);
			y = std::min(cthFrame->height-1,y);
			if(x != 0 && y !=0 && roisize > 0){
				cvSetImageROI( cthFrame, cvRect(x,y,roisize*2,roisize*2) );
				cvXorS( cthFrame, cvScalarAll(255), cthFrame, 0 );
			}
			
            cvResetImageROI(cthFrame);
			cvShowImage("backProject", cthFrame);
        }

        cvShowImage("CamShiftDemo", original);
		

		
        loop = checkKeys(track, cth);
    }
    // this guarenties that the thread exits without errors
    g.exit();
    tG.join();
    delete cth;
    return 0;
}
