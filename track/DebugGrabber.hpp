#pragma once
#include "Grabber.h"
#include "Timer.h"
#include <iostream>

class DebugGrabber : public Grabber
{
    Timer t;
    bool started;
    int framecount;
public:

    DebugGrabber(const int camId, const CvSize resolution, TripleBuffering& buffer)
        : Grabber(camId, resolution, buffer) , started(false) , framecount(0)
    {
    }

    virtual void beforeGrab() throw() {
        if(!started){
            started = true;
            t.start();
        }
        ++framecount;
        if(framecount == 60){
            double d = t.elapsed();
            std::cout << 1 / ((d / 1000) /framecount ) << std::endl;
			framecount = 0;
            t.restart();
        }
    };
    virtual void afterGrab() throw(){};
};
