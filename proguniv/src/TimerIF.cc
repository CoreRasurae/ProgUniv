/*
 * TimerIF.cc
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */
#include "../include/TimerIF.hh"
#include "../include/TimerStdStrategy.hh"
#include "../include/TimerExtendedStrategy.hh"

using namespace InterfaceClass;

void InterfaceClass::TimerIF::createTimer(std::string timerType) {
	if (timerType == "STD") {
		TimerIFStatic::timer = new TimerStdStrategy();
	} else {
		TimerIFStatic::timer = new TimerExtendedStrategy();
	}
}

InterfaceClass::TimerIF& InterfaceClass::TimerIF::getInstance() {
	return *TimerIFStatic::timer;
}

void InterfaceClass::TimerIF::delay(uint64_t useconds) {
	//Log info, enter realtime?
	delay_tsc(useconds);
}




