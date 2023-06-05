/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ChipATMega88.hh
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

#ifndef CHIPATMEGA48PA_HH_
#define CHIPATMEGA48PA_HH_

#include "../include/ChipIF.hh"
#include "../include/ChipATMega48.hh"

/**
 * The implementation with the ATMEL AVR ATMega48PA characteristics
 * Abstract representation of the chip memories and configuration parameters 
 */
class ChipATMega48PA : public ChipATMega48 {
	public:
		static ChipIF *createInstance(const std::string &deviceSpecification) {
			return new ChipATMega48PA(deviceSpecification);
		}
		static string getFactoryNameReference() {
			return "ATMEGA48PA";
		}
		~ChipATMega48PA() {};
		
	protected:
		ChipATMega48PA(const std::string &deviceSpecification) : ChipATMega48("PA") {};
        
	private:
    	ChipATMega48PA& operator=(const ChipATMega48PA& rhs);
};


#endif /* CHIPATMEGA48PA_HH_ */
