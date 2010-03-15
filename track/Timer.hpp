
#ifndef __TIMER_H__
#define __TIMER_H__

#include <windows.h>
#undef max
#undef min

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
				double mls = static_cast<double>(current.QuadPart - start_.QuadPart) / static_cast<double>(frequency_.QuadPart);
				start_ = current;
				return mls;
			}
			return 0;
		}

		void AccurateSleep(DWORD milliSeconds) {
			static LARGE_INTEGER s_freq = {0, 0};
			if (s_freq.QuadPart == 0) {
				QueryPerformanceFrequency(&s_freq);
			}
			LARGE_INTEGER from, now;
			QueryPerformanceCounter(&from);
			int ticks_to_wait = static_cast<int>(s_freq.QuadPart) / (1000 / milliSeconds);
			bool done = false;
			int ticks_passed;
			int ticks_left;

			do {
				QueryPerformanceCounter(&now);
				ticks_passed = static_cast<int>((__int64)now.QuadPart - (__int64)from.QuadPart);
				ticks_left = ticks_to_wait - ticks_passed;

				if (now.QuadPart < from.QuadPart) {    // time wrap
					done = true;
				}
				if (ticks_passed >= ticks_to_wait) {
					done = true;
				}

				if (!done) {
					if (ticks_left > static_cast<int>(s_freq.QuadPart * 2 / 1000)) {
						Sleep(1);
					} else {
						for (int i = 0; i < 10; i++) {
							Sleep(0); 
						}
					}
				}
			} while (!done);
		}

	private:
		LARGE_INTEGER frequency_;
		LARGE_INTEGER start_;
		
		bool running_;
};

#endif
