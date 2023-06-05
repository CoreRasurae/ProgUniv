/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            RealTime.cc
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

#ifdef __linux__
#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#elif __WIN32__
#include <windows.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/RealTime.hh"
#include "../include/RTModeException.hh"


RealTime::RealTime() {
#ifdef __linux__
	normal_sched_policy = sched_getscheduler(0);
	sched_getparam(0, &normal_sp);
#elif  __WIN32__
#else
#error "Platform not supported"
#endif
	modo_realtime = false;
	disabled = false;
}

RealTime::~RealTime() {
}

RealTime *RealTime::instance = 0;

void RealTime::setDisabled(bool newState) {
	disabled = newState;
}

RealTime& RealTime::getInstance() {
	if (instance == 0)
		instance = new RealTime();
	return *instance;
}

void RealTime::enterRTMode() {
	if (disabled) {
		if (modo_realtime) {
			leaveRTMode();
		}

		return;
	}

#ifdef __linux__
	struct sched_param sp;
	if (!modo_realtime) {
		//sp.sched_priority = sched_get_priority_min(SCHED_FIFO);
		sp.sched_priority = sched_get_priority_max(SCHED_BATCH);
		//sp.sched_priority = 1;
		
		//if (sched_setscheduler(0, SCHED_FIFO, &sp) < 0) {
		if (sched_setscheduler(0, SCHED_BATCH, &sp) < 0) {
			perror("RealTime enterRTMode()");
			exit(1);
		}

		modo_realtime = true;
	}
#elif __WIN32__
	uint32_t error, priClass;
	if (!modo_realtime) {
	   if(!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
	   {
	      error = GetLastError();
	      if( HIGH_PRIORITY_CLASS != error) {
	    	  throw RTModeException("Failed to enter in RealTime.");
	      }
	   }

	   // Display priority class
	   priClass = GetPriorityClass(GetCurrentProcess());
	   if (priClass != HIGH_PRIORITY_CLASS) {
		   throw RTModeException("Failed to enter in RealTime.");
	   }

	   modo_realtime = true;
	}
#else
#error "Unsupported platform"
#endif
}

void RealTime::leaveRTMode() {
#ifdef __linux__
	if (modo_realtime) {
		if (sched_setscheduler(0, normal_sched_policy, &normal_sp) < 0) {
			perror("RealTime leaveRTMode()");
			exit(1);
		}

		modo_realtime = false;
	}
#elif __WIN32__
	uint32_t error, priClass;

	if (modo_realtime) {
	   if(!SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS))
	   {
		  error = GetLastError();
		  if( NORMAL_PRIORITY_CLASS != error) {
			  throw RTModeException("Failed to enter in RealTime.");
		  }
	   }

	   // Display priority class
	   priClass = GetPriorityClass(GetCurrentProcess());
	   if (priClass != NORMAL_PRIORITY_CLASS) {
		   throw RTModeException("Failed to enter in RealTime.");
	   }

	   modo_realtime = false;
	}
#else
#error "Unsupported platform"
#endif
}

bool RealTime::getRTMode() {
	return modo_realtime;
}
