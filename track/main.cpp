
#include <iostream>
#include <windows.h>

#include <boost/thread.hpp>

#include "Grabber.h"
#include "DebugGrabber.hpp"
#include "TripleBuffering.h"
#include "ColorThreshold.h"
#include "Tracker.h"
#include "TimedImage.hpp"

#ifndef _EiC
	#include "cv.h"
	#include "highgui.h"
#endif

using namespace std;
ColorThreshold *colorThreshold;
bool show_hist = 1;
CvPoint origin_;
CvRect selection_;
bool select_object_ = false;
CvSize resolutionGrab = cvSize(320, 240);
CvSize resolutionProcess = cvSize(160, 120);
double scale = 0.5;

void on_mouse_event(int event, int x, int y, int flags, void* param ) {
	x = resolutionGrab.width -x;
	if (select_object_) {
		selection_.x = std::min(x, origin_.x);
		selection_.y = std::min(y, origin_.y);
		selection_.width = selection_.x + abs(x - origin_.x);
		selection_.height = selection_.y + abs(y - origin_.y);
		selection_.x = std::max(selection_.x, 0);
		selection_.y = std::max(selection_.y, 0);
		selection_.width = std::min(selection_.width, resolutionGrab.width);
		selection_.height = std::min(selection_.height, resolutionGrab.height);
		selection_.width -= selection_.x;
		selection_.height -= selection_.y;
	}
	switch (event) {
		case CV_EVENT_LBUTTONDOWN:
			origin_ = cvPoint(x, y);
			selection_ = cvRect(x, y, 0, 0);
			select_object_ = true;
			break;
		case CV_EVENT_LBUTTONUP:
			select_object_ = false;
			if (selection_.width > 0 && selection_.height > 0 ) {
				colorThreshold->calcule_hist_ = true;
				colorThreshold->track_object_ = false;
				CvRect selection = selection_;
				selection.x = int(selection.x * scale);
				selection.y = int(selection.y * scale);
				selection.width = int(selection.width * scale);
				selection.height = int(selection.height * scale);
				colorThreshold->selection_ = selection;
			}
			break;
	}
}

bool checkKeys(Tracker &t, ColorThreshold* ct) {
    bool retval = true;
    int c = cvWaitKey(10);
    switch(static_cast<char>(c)) {
		case 27:
			retval = false;
			break;
		case 'b':
			t.backproject_mode_ ^= 1;
			break;
		case 'c':
			ct->turnOffTracking();
			break;
    }
    return retval;
}

void printMenu() {
    printf( "Hot keys: \n"
        "\tESC - quit the program\n"
        "\tc - stop the tracking\n"
        "\tb - switch to/from backprojection view\n"
        "To initialize tracking, select the object with mouse\n" );
}

int main(int argc, char** argv) {	
    //initialize fps limiting variable
	const int FRAMES_PER_SECOND = 30;
	const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;
	DWORD next_game_tick = timeGetTime();

	char static_array[60];
	//get printf to use a statically defined buffer to prevent it to call malloc()
	setvbuf(stdout, static_array, _IOLBF, sizeof(static_array));
	cvNamedWindow("CamShiftDemo", 1);
	cvNamedWindow("backProject", 1);
	TripleBuffering threadBuffer(resolutionGrab, true);
	TripleBuffering threadBuffer2(resolutionProcess, true); 
    Grabber grabber(0, resolutionGrab, threadBuffer,threadBuffer2);
	Tracker track(resolutionProcess);
	colorThreshold = new ColorThreshold(resolutionProcess,threadBuffer2,track);
	boost::thread threadGrabber(&Grabber::operator(), &grabber);
	boost::thread threadColorThreshold(&ColorThreshold::operator(), colorThreshold);

    cvSetMouseCallback("CamShiftDemo", on_mouse_event, 0);
    printMenu();
    IplImage* original = NULL;
	IplImage* renderFrame = cvCreateImage(resolutionGrab, 8, 3);
	Timer t;
	t.start();
	unsigned int framecount = 0;
    unsigned int meanDelay  = 0;
	bool loop = true;

    while (loop) {

        // This section will probably change in the future.
		next_game_tick += SKIP_TICKS;
		int sleep_time = next_game_tick - timeGetTime();
		TimedImage tmp;
		if( sleep_time > 0 ) {
			tmp = threadBuffer.read(sleep_time);
		}else {
			tmp = threadBuffer.read(0);
		}
        cvCopyImage(tmp.image_,renderFrame);
		if ( select_object_ &&
			 selection_.width > 0 &&
			 selection_.height > 0) {
     	    cvSetImageROI(renderFrame,selection_);
			cvXorS(renderFrame, cvScalarAll(255), renderFrame, 0);
			cvResetImageROI(renderFrame);
		}

        if (track.backproject_mode_ && colorThreshold->track_object_ ) {
			Circle filtered = track.getNext();
			filtered.x_ =  float(filtered.x_ / scale);
			filtered.y_ =  float(filtered.y_ / scale);
			filtered.radius_ = float(filtered.radius_ / scale);

			const int RADIUS = static_cast<int>(filtered.radius_);
            if (RADIUS > 0 && RADIUS < 32768) {
				const CvScalar CIRCLE_SCALAR = CV_RGB(255, 0, 0);
                cvCircle(renderFrame, filtered.getCenter(), RADIUS, CV_RGB(255, 0, 0), 2);
                cvCircle(renderFrame, filtered.getCenter(), 1, CV_RGB(255, 0, 0), -1, CV_AA, 0);
            }
        }
		cvFlip(renderFrame,renderFrame,1);

		//wait for the right time to show the frame
		sleep_time = next_game_tick - timeGetTime();
		if (sleep_time > 0){
			Timer::AccurateSleep(sleep_time/2);
		}

         // display stats
		DWORD ttime = timeGetTime();
        meanDelay += (ttime - tmp.timeStamp_);
        
		if (++framecount == 120) {
            double d = t.elapsed();
            //calculate the rendering fps
			printf("Display: %.2f fps\n",1 / ((d / 1000) / framecount ));
			fflush(stdout);
            
            //jitter
           	printf("Display: %d jitter\n",next_game_tick - ttime);
			fflush(stdout);
		    printf("Delay: %d ms\n", meanDelay/120);
            meanDelay = 0;
		    fflush(stdout);        //Show the rendered frame now
            framecount = 0;
        }
		

		cvShowImage("CamShiftDemo", renderFrame);

		loop = checkKeys(track, colorThreshold);
    }

	cvReleaseImage(&renderFrame);

    // The call to exit() garanties that the thread exits without error    
	colorThreshold->exit();
	threadColorThreshold.join();

	grabber.exit();
	threadGrabber.join();
    delete colorThreshold;

    return 0;
}
