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
    //CvSize resolution = cvSize(176, 144);
    CvSize resolution = cvSize(320, 240);
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
	Timer t;
	t.start();
    unsigned int framecount = 0;
    while(loop) {
		if (++framecount == 60) {
			double d = t.elapsed();
			std::cout << "Display: " << 1 / ((d / 1000) / framecount ) << "fps" << std::endl;
			framecount = 0;
			t.restart();
		}
        // this section will probably change in the future.
        original = threadBuffer.read();
        cthFrame = cth->process( original );

        if( track.backproject_mode_ && cth->track_object_ ){
            Circle filtered = track.process(cthFrame);
            if(int(filtered.radius_) > 0 && filtered.radius_ <  32768){
                cvCircle(original,filtered.center_,filtered.radius_,CV_RGB(255,0,0),2);
                cvCircle(original,filtered.center_, 1,CV_RGB(255,0,0), -1, CV_AA, 0);
            }
            /*
			Circle truecircle = track.filtered;
			if(int(truecircle.radius_) > 0 && truecircle.radius_ <  32768){
				cvCircle(original,truecircle.center_,truecircle.radius_,CV_RGB(0,255,0),2);
				cvCircle(original,truecircle.center_, 1,CV_RGB(0,255,0), -1, CV_AA, 0);
			}
			*/
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
