/*
 * TimerExtendedStrategy.hh
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */

#ifndef TIMEREXTENDEDSTRATEGY_HH_
#define TIMEREXTENDEDSTRATEGY_HH_

#include "TimerIF.hh"
#include <string>
#include <exception>
#include "UnsupportedParameterException.hh"
#include "NotImplementedException.hh"

#ifdef __linux__



#elif __WIN32__

#include <windows.h>
#include <winbase.h>

#else
#error "Unsupported Platform"
#endif

using namespace std;

class TimerExtendedStrategy : public InterfaceClass::TimerIF {
private:
	bool failedToGetTimer;
	uint64_t frequency;
	uint64_t avgCyclesPerUs;

public:
	TimerExtendedStrategy() {
		frequency = 0;
		avgCyclesPerUs = 0;
		failedToGetTimer = false;

#ifdef __WIN32__
		LARGE_INTEGER temp;
		if (!QueryPerformanceFrequency(&temp)) {
			failedToGetTimer = true;
			return;
		}

		frequency = temp.QuadPart;

		avgCyclesPerUs = frequency/1000000UL;
#endif
	}

	/*extern "C" {
	   __inline__ u_int64_t rdtsc64() {
	   u_int32_t lo, hi;
	   // We cannot use "=A", since this would use %rax on x86_64
	   __asm__ __volatile__ ( "xor %%eax, %%eax\n "
			                  "cpuid\n "
			   "              rdtsc" : "=a" (lo), "=d" (hi)::"%ebx","%ecx");
	   return (u_int64_t)hi << 32 | lo;
	   }
	}*/

	virtual uint64_t getCurrentInstant() {
		uint64_t count = 0;
#ifdef __WIN32__
		LARGE_INTEGER temp;
		QueryPerformanceCounter(&temp);
		count = temp.QuadPart;
#elif __linux__
		//return rdtsc64();
#else
#error "Unsupported Platform"
#endif

		return count;
	}

	virtual void nonActiveLowPrecisionDelay(uint64_t delay_us) {
#ifdef __WIN32__
		if (delay_us < 1000) {
			throw UnsupportedParameterException("Delay is below 1ms");
		}
		Sleep(delay_us / 1000);
#elif __linux__
#else
#error "Unsupported Platform"
#endif
	}

	virtual ~TimerExtendedStrategy() {}

protected:
	virtual void delay_tsc(uint64_t delay_us) {
#ifdef __WIN32__
		uint64_t startCount, endCount;
		LARGE_INTEGER tempA;
		if (failedToGetTimer) {
			throw UnsupportedParameterException("Timer not available");
		}

		endCount = delay_us * avgCyclesPerUs;
		QueryPerformanceCounter(&tempA);
		endCount += tempA.QuadPart;
		do {
			QueryPerformanceCounter(&tempA);
		} while (tempA.QuadPart < endCount);
#elif __linux__
		throw NotImplementedException("Not implemented");
#endif
	}
};


#endif /* TIMEREXTENDEDSTRATEGY_HH_ */
