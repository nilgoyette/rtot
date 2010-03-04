
#ifndef __TIMER_H__
#define __TIMER_H__

#include <windows.h>

class Timer {
	public:
		Timer(void) throw();

		void start(void) throw();
		void restart(void) throw();

		void pause(void) throw();
		void resume(void) throw();

		void reset(void) throw();

		double elapsed(void) throw();

	private:
		LARGE_INTEGER frequency_;
		LARGE_INTEGER start_;
		
		bool running_;
};

#endif
