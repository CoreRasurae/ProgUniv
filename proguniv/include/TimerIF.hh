/*
 * TimerIF.hh
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */

#ifndef TIMERIF_HH_
#define TIMERIF_HH_

#include <cinttypes>
#include <memory>

namespace InterfaceClass {
	class TimerIF {
	public:
		static void createTimer(std::string timerType);

		static TimerIF& getInstance(); //Access the singleton

		/**
		 * High Precision Delay for a given number of microseconds
		 * @param useconds number of microseconds to delay the execution
		 */
		void delay(uint64_t useconds);

		/**
		 * Perfoms a non active time wait using a lower precision timer, but saving system resources
		 * Normally only milisecond precision can be achieved
		 * @param us_delay number of microseconds to delay the execution
		 */
		virtual void nonActiveLowPrecisionDelay(uint64_t us_delay) = 0;

		/**
		 * Returns the value of a microsecond counter that represents the
		 * current instant
		 * @return current instant in microseconds
		 */
		virtual uint64_t getCurrentInstant() = 0; //Current on time aprox. in us

		virtual ~TimerIF() {}

	protected:
		virtual void delay_tsc(uint64_t delay_us) = 0;

		TimerIF() {}
		TimerIF(const TimerIF& rhs) {}

	private:
		TimerIF& operator=(const TimerIF& rhs);
	};

    namespace TimerIFStatic {
    	static TimerIF *timer;
    }
}


#endif /* TIMERIF_HH_ */
