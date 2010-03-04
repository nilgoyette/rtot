
#ifndef __TIMER_H__
#define __TIMER_H__

#include <windows.h>

class Timer {
	public:
		Timer::Timer(void) throw() : running_(false) {
			start_.QuadPart = 0;
			QueryPerformanceFrequency(&frequency_);
			SetThreadAffinityMask(GetCurrentThread(), 1);
			frequency_.QuadPart /= 1000;
		}

		void Timer::start(void) throw() {
			if (!running_) {
				QueryPerformanceCounter(&start_);
				running_ = true;
			} else {
				reset();
			}
		}

		void Timer::restart(void) throw() {
			reset();
			start();
		}

		void Timer::reset(void) throw() {
			start_.QuadPart = 0;
			running_ = false;
		}

		double Timer::elapsed(void) throw() {
			if (start_.QuadPart) {
				LARGE_INTEGER current;
				QueryPerformanceCounter(&current);
				double mls = (double)(current.QuadPart - start_.QuadPart) / (double)frequency_.QuadPart ;
				start_ = current;
				return mls;
			}
			return 0;
		}

	private:
		LARGE_INTEGER frequency_;
		LARGE_INTEGER start_;
		
		bool running_;
};

#endif
