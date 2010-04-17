
#ifndef __TIMER_H__
#define __TIMER_H__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#undef max
#undef min
#pragma comment(lib, "winmm.lib")

class Timer {
	public:
		Timer::Timer(void) throw() : running_(false) {
            SetProcessAffinityMask(GetCurrentProcess(),1);
			SetThreadAffinityMask(GetCurrentThread(), 1);
			timeBeginPeriod(1);
		}

		void Timer::start(void) throw() {
			if (!running_) {
				start_ = timeGetTime();
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
			start_ = timeGetTime();
			running_ = false;
		}

		double Timer::elapsed(void) throw() {
			if (running_) {
				DWORD current = timeGetTime();
				double mls =  (current-start_)*1.0f;
                start_ = current;
				return mls;
			}
			return 0;
		}

		// 1ms resolution on windows xp and about 8.0 us overhead
		static void Timer::AccurateSleep(DWORD milliSeconds) {
			DWORD from, now;
			from = timeGetTime();
			bool done = false;
			DWORD ticks_passed;
			do {
				now =  timeGetTime();
				ticks_passed = now-from;
				if (now < from) {    // time wrap
					done = true;
				}
				if (ticks_passed >= milliSeconds) {
					done = true;
				}
			} while (!done);
		}

		Timer::~Timer(void){
			timeEndPeriod(1);
		}
	private:
		DWORD start_;
		bool running_;
};

#endif
