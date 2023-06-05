/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            Timer.cc
 *
 *  Copyright  2007,2008  Lu�s Mendes
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
#include <chrono>
#include <ostream>
#include <ctime>
#include <cstdio>
#include <ios>
#include <locale>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "../include/TimerStdStrategy.hh"
#include "../include/RealTime.hh"

extern int errno;

using namespace std::chrono;
using std::ios;
using namespace std;

TimerStdStrategy::TimerStdStrategy()
{
	tsc_measures_counter = 0;
	tsc_accum_cycles = 0;
	tsc_old_accum_cycles = 0;
	tsc_cycles = 0;
	tsc_average_cycles = 0;

	use_tsc = true;
	disable_nanosleep = false;
}

TimerStdStrategy::~TimerStdStrategy()
{
}

/*extern __inline__ unsigned long long int rdtsc()
{
    unsigned long long int x;
    __asm__  (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}*/

extern "C" {
   __inline__ uint64_t rdtsc() {
   uint32_t lo, hi;
   // We cannot use "=A", since this would use %rax on x86_64
   __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
   return (uint64_t)hi << 32 | lo;
   }
}


void TimerStdStrategy::initTimer() {
#ifdef __DISABLED__
	struct timespec ts;

	//Maximum DELTA error from minimum value sample to maximum value sample
	const int MAX_SAMPLES_DELTA = 7000;

	uint64_t start_cycles, diff_cycles, end_cycles;
	uint64_t local_accum_cycles, local_measures_count;
	uint64_t local_estimated_cycles;

	uint64_t diff;
	int i;

	bool em_tempo_real; //Flag que memoriza o estado do modo de tempo real
	bool unsynchronized_tsc_cores = false;


	//Find how much clock cycles are generated for a 1Sec interval
	local_accum_cycles = 0;

	//Let's use the the OS timer to calibrate the cycles
	uint64_t start_time, end_time;

	//Build a list with the values
	uint64_t measure_table[1000];
	uint64_t min_val = 0, max_val = 0;
	uint64_t min_accepted_val = 0, max_accepted_val = 0;

	const int sample_size = 100; //Total time equals sample_size * interval
	unsigned const int interval = 10000; //in useconds (must not exceed 1second or 1000000 useconds)

	/*RealTime& instance = RealTime::getInstance();
	em_tempo_real = instance.getRTMode();
	if (!em_tempo_real)
		instance.enterRTMode();*/

	float percent_of_samples;
	if (use_tsc) {
		for (i = 0; i < sample_size; i++) {
	        	measure_table[i] = 0;
	        }

		//Number of times the clock went backwards
		int failed_tries = 0;
		int passes = 0;

		int lower = 0, upper = sample_size - 1;
		uint64_t delta;

		int current_pos = 0;
		while ( passes < 5 && percent_of_samples < 0.50 ) {
			for (i = 0; i < sample_size; i++) {
				start_time = end_time = getOSCurrentInstant();
				start_cycles = rdtsc();
				while (end_time - start_time < interval) //delay for the time of an interval (in useconds)
				{
					end_cycles = rdtsc();
					end_time = getOSCurrentInstant();
				}

				if (end_time - start_time > interval + 100) {
					cout << "Unable to calculate estimated clock cycles, 1msec delay was exceeded by " << ((int)(end_time - start_time) - interval) << " useconds" << endl;
					exit(100);
				}

				diff_cycles = end_cycles - start_cycles;
				if (diff_cycles > 0) {
					if ( measure_table[i] == 0 ) {
						measure_table[i] = diff_cycles;
					}
					else if (diff_cycles >= (min_accepted_val - (MAX_SAMPLES_DELTA/passes)) && diff_cycles <= (max_accepted_val + (MAX_SAMPLES_DELTA/passes))) {
						if (current_pos == lower)
							current_pos = upper + 1;
						if (current_pos >= sample_size)
							break;

						measure_table[current_pos++] = diff_cycles;
					}

					if (min_val == 0 || diff_cycles < min_val)
						min_val = diff_cycles;

					if (max_val == 0 || diff_cycles > max_val)
						max_val = diff_cycles;
				}
				else {
					//Repeat until we get a more useful value
					i--;

					//Stop if maxtries is reached
					failed_tries++;
					if (failed_tries > sample_size) {
						cout << "Failed to recover from unsynchronized clocks" << endl;
						exit(100);
					}
				}
			}

			//Order the array by ascending order
			sort(measure_table, &measure_table[sample_size]);

			//Find the best fit
			lower = 0;
			upper = sample_size - 1;
			delta = measure_table[upper] - measure_table[lower];
			while (delta > MAX_SAMPLES_DELTA) {
				delta = measure_table[upper - 1] - measure_table[lower];

				if (upper - lower == 2)
					break;

				upper--;
			}


			local_measures_count = upper - lower + 1;
			percent_of_samples = (float)local_measures_count / (float)sample_size;

			if ( percent_of_samples < 0.50 ) {
				cout << "AQUI!!!!!!KKKK" << endl;
				//Now, let's try to maximize the result obtained, by shifting the lower interval
				int max_size_lower_index = lower;
				int max_size_upper_index = upper;
				int step = 0;
				int displacement = 1;
				int temp_upper = upper + displacement;
				int temp_lower = lower + displacement;
				bool increased;
				while ( temp_upper < sample_size ) {
					step = 0;
					increased = false;
					while ( (measure_table[temp_upper] - measure_table[temp_lower - step] < MAX_SAMPLES_DELTA) ) {
						step++;
						increased = true;
					}

					if ( increased && (temp_upper - temp_lower + step) > (max_size_upper_index - max_size_lower_index) ) {
						max_size_lower_index = temp_lower - step;
						max_size_upper_index = temp_upper;
					}

					displacement++;
					temp_upper = upper + displacement;
					temp_lower = lower + displacement;
				}

				if (max_size_lower_index != 0) {
					cout << "Best fit starts at index " << max_size_lower_index << " and contains " <<
						(max_size_upper_index - max_size_lower_index + 1) << " samples." << endl;
						cout << "Original first fit was " << (upper - lower + 1) << " samples long." << endl;
					upper = max_size_upper_index;
					lower = max_size_lower_index;
				}
			}

			local_measures_count = upper - lower + 1;
			percent_of_samples = (float)local_measures_count / (float)sample_size;

			min_accepted_val = measure_table[lower];
			max_accepted_val = measure_table[upper];
			passes++;
		}

		local_accum_cycles = 0;
		for (i = lower; i <= upper; i++) {
			local_accum_cycles += measure_table[i];
		}

		//Average measured cpu clock cycles for a period of an interval
		local_estimated_cycles = local_accum_cycles / local_measures_count;
		//Adjust cycles for a microsecond
		tsc_cycles = local_estimated_cycles / interval;

		cout << "Delta is: " << delta << " number of occurrences: " << local_measures_count << endl;
		cout << "Min value is: " << measure_table[lower] << " Max value is: " << measure_table[upper] << " Backwards: " << failed_tries << endl;
		cout << "Maximum offset is: " << max_val-min_val << endl;


		if (percent_of_samples < 0.50) {
			cout << "Percentage of samples within accepted margin is insufficient (< 50%)" << endl;
			exit(100);
		}
		cout << "Percentage of samples within accepted margin is: " << (percent_of_samples*100.0) << "% in " << passes << " convergence steps" << endl;

		if ( (double)(max_val - min_val) / (double)(measure_table[upper] - measure_table[lower]) > 1.001) {
			cout << "CPU core clocks seem to be unsynchronized, but have been software compensated!" << endl;
			cout << "Some precision is lost and fine tuning won't take place" << endl;
			unsynchronized_tsc_cores = true;
		}

		cout << "1sec in processor cycles = " << local_estimated_cycles * (1000000 / interval) << endl;

		cout << "processor cycles for 1 us = " << tsc_cycles << endl;

		//Test nanosleep precision, if ok, use nanosleep instead
		start_cycles = rdtsc();
		delay(1);
		diff = rdtsc() - start_cycles;

		if ( unsynchronized_tsc_cores) {
			if ( diff > tsc_cycles + (max_val - min_val) ) {
				cout << "nanosleep too slow using software delay loop - expected = " <<
					 tsc_cycles << " obtained " << diff << endl;

				disable_nanosleep = true;
			}
		}
		else {
			if ( diff > tsc_cycles + 10 ) {
				cout << "nanosleep too slow using software delay loop - expected = " <<
					 tsc_cycles << " obtained " << diff << endl;

				disable_nanosleep = true;
			}
		}

		if (!unsynchronized_tsc_cores && disable_nanosleep) {
			//So we are using rdtsc, let's simulate a delay to see if cycle compensation is required
			start_cycles = rdtsc();
			end_cycles = start_cycles + tsc_cycles;
			while (rdtsc() < end_cycles);
			diff = rdtsc() - start_cycles;
			cout << "Initial delay took = " << diff << " cycles" << endl;

			if (diff - tsc_cycles > 0) {
				//Cycles fine calibration is required
				cout << "Fine calibrating delay" << endl;

				//Let's extract the extra cycles (diff) from the exact cycles expected
				//to compensate software code overhead
				tsc_cycles -= (diff - tsc_cycles);

				//Let's see what we've got
				start_cycles = rdtsc();
				end_cycles = start_cycles + tsc_cycles;
				while (rdtsc() < end_cycles);
				diff = rdtsc() - start_cycles;
				cout << "Calibrated delay took = " << diff << " cycles" << endl;
			}
			else {
				cout << "Unable to fine calibrate delay - software loop too slow" << endl;
			}
		}
	}
	else {
		//Let's test the clock_gettime in realtime mode
		cout << "Clocks per sec: " << CLOCKS_PER_SEC << endl;
		//timespec tv_sec e tv_nsec
		//Let's extract the clock resolution
		clock_getres(CLOCK_MONOTONIC, &ts);
		cout << "Seconds: " << ts.tv_sec << " Nanoseconds: " << ts.tv_nsec << endl;


		struct timespec ts_end;
		clock_gettime( CLOCK_MONOTONIC, &ts );
		delay(1);
		clock_gettime( CLOCK_MONOTONIC, &ts_end );

		cout << "Diferen�a: " << ts_end.tv_nsec - ts.tv_nsec << endl;
	}
#endif

	/*if (!em_tempo_real)
		instance.leaveRTMode();*/
}

__inline__ uint64_t TimerStdStrategy::getOSCurrentInstant() {
#ifdef __linux__
	struct timespec ts;

	int status = clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts);
	if (status != 0)
	{
		perror("clock_gettime");
		exit(123);
	}


	return (uint64_t)( (ts.tv_sec * 1000000) + (ts.tv_nsec / 1000.0) ); //Return time in useconds
#else
	return 0;
#endif
}

uint64_t TimerStdStrategy::getCurrentInstant() {
	//TODO Commented out until a proper solution is found, this way we avoid the unsychronized clock problem at the cost of precision
	//if (use_tsc)
	//	return (uint64_t)(rdtsc() / tsc_cycles);
	//else {
		return getOSCurrentInstant();
	//}
}

void TimerStdStrategy::delay_tsc(uint64_t usDelay) {
	using std::dec;
	using std::endl;
	uint64_t startCycles, endCycles, remaining;

	microseconds d = (microseconds)usDelay;
	//time_point<high_resolution_clock, microseconds>??=
	auto t = high_resolution_clock::now() + d;
	while (high_resolution_clock::now() < t);
}


void TimerStdStrategy::nonActiveLowPrecisionDelay(uint64_t us_delay) {
#ifdef __linux__
	struct timespec ts, remts;

	ts.tv_sec = us_delay / 1000000;
	ts.tv_nsec = (us_delay - ts.tv_sec*1000000) * 1000;

	bool cont = true;
	while (cont) {
		if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &remts) < 0) {
			if (errno == EINTR) {
				ts = remts;
			}
			else {
				cont = false;
			}
		}
		else
			cont = false;
	}
#endif
}
