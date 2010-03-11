
#ifndef __DEBUG_GRABBER_H__
#define __DEBUG_GRABBER_H__

#include <iostream>
#include "Grabber.h"
#include "Timer.hpp"

class DebugGrabber : public Grabber {
	public:
		DebugGrabber(const int camId, const CvSize resolution, TripleBuffering& buffer)
			: Grabber(camId, resolution, buffer) , started(false) , framecount(0) {
		}

		virtual void beforeGrab() throw() {
			if (!started) {
				started = true;
				t.start();
			}
			
			if (++framecount == 10) {
				double d = t.elapsed();
				std::cout << "FPS : " << 1 / ((d / 1000) / framecount ) << std::endl;
				framecount = 0;
				t.restart();
			}
		};

	private:
		Timer t;
		bool started;
		int framecount;
};

#endif
