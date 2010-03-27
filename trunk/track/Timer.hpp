
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
		static __inline LARGE_INTEGER Timer::rdtsc() {
			LARGE_INTEGER retval;
			__asm {
				CPUID 
				RDTSC
					mov retval.HighPart, edx
					mov retval.LowPart, eax
			}
			return retval;
		}
		static __inline double Timer::rdtscElapsed(LARGE_INTEGER t1, LARGE_INTEGER t0,double cpuspeed)
		{  
			return (double)(t1.QuadPart - t0.QuadPart)/cpuspeed;
		}

		static __inline double Timer::getCpuSpeed()
		{  
			LARGE_INTEGER b = rdtsc();
			AccurateSleep(1000);
			LARGE_INTEGER e = rdtsc();
            return (e.QuadPart-b.QuadPart)/1000.0f;
		}

		static void Timer::AccurateSleeprdtsc(DWORD milliSeconds,double cpuspeed) {
			LARGE_INTEGER from, now;
			from = rdtsc();
			bool done = false;
			double ticks_passed;
			do {
				now =  rdtsc();
				ticks_passed = rdtscElapsed(now,from,cpuspeed);
				if (ticks_passed >= milliSeconds) {
					done = true;
				}
			} while (!done);
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
