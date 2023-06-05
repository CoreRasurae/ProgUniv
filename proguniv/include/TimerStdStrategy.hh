/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            Timer.hh
 *
 *  Copyright  2007  Luís Mendes
 *  Email
 ****************************************************************************/

/*
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TIMERSTDSTRATEGY_HH_
#define _TIMERSTDSTRATEGY_HH_

#include <stdint.h>
#include "TimerIF.hh"

/**
 * This class provides u-second precision timing for use within the real-time application.
 * For now only CPU Time Stamp Counter (TSC) based timing is available. For Dual Core CPUs both
 * Intel and AMD special care must be taken, as for these CPUs it's quite common that each core
 * has a different value in it's own TSC. Such situation may arise from the following:
 *  - Each core was initialized and started at diferent instants
 *  - A core stopped to save power consumption and started latter
 *  - One core changed to a lower frequency for power efficiency and
 *    the other remained working at another frequency
 * The first two cases are detected and software compensated by the Timer class internal methods
 * for any number of processor(s)/core(s) available, however it is certain that some accuracy will
 * be lost when comparing to an AMD K7 processor or an Intel P4 processor.
 * 
 * For the third case, there are two options and one of them requires end-user intervention,
 * that is, check /proc/cpu_info and if the processor has the constant_tsc feature, if so,
 * you don't have to worry because the TSCs are incremented at a constant frequency indepently
 * of the core throttling state. If not, make sure you configure your machine for full performance
 * or maximum power save to see if you can lock the cores throttling states during the excution of
 * this software.  
 */
class TimerStdStrategy : public InterfaceClass::TimerIF
{
	public:
		TimerStdStrategy();

		virtual ~TimerStdStrategy();

		/**
		 * Perfoms a non active time wait using a lower precision timer, but saving system resources
		 * Normally only milisecond precision can be achieved
		 * @param us_delay number of microseconds to delay the execution
		 */
		virtual void nonActiveLowPrecisionDelay(uint64_t us_delay);
		
		/**
		 * Returns the value of a microsecond counter that represents the
		 * current instant
		 * @return current instant in microseconds 
		 */
		virtual uint64_t getCurrentInstant(); //Current on time aprox. in us
		
	protected:
		/**
		 * Performs small delays using active wait based on calibrated/compensated TSC values.
		 * For high delays (1Sec and above) the process will be suspended until the time is
		 * elapsed.
		 * @param useconds number of micro-seconds to delay
		 */
		virtual void delay_tsc(uint64_t delay_us);

	private:
		/**
		 * Obtains the current instant in useconds using the internal OS timer
		 * Typically the internal OS timer has a relatively low update rate
		 * @return the number of useconds elapsed
		 */
		uint64_t getOSCurrentInstant();
		
		//Cycles contains the number of clock cycles corresponding to 1usec
		uint64_t tsc_cycles, tsc_average_cycles;
		//Variables used to update the average_cycles
		uint64_t tsc_measures_counter, tsc_old_accum_cycles, tsc_accum_cycles;
		bool disable_nanosleep;
		
		bool use_tsc; //Flag that indicates if the TSC should be used
		
		void initTimer();
		
};

#endif //_TIMERSTDSTRATEGY_HH_
