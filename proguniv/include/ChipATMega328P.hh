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

#ifndef CHIPATMEGA328P_HH_
#define CHIPATMEGA328P_HH_

#include "../include/ChipIF.hh"
#include "../include/ChipATMega328.hh"

/**
 * The implementation with the ATMEL AVR ATMega168A characteristics
 * Abstract representation of the chip memories and configuration parameters 
 */
class ChipATMega328P : public ChipATMega328 {
	public:
		static ChipIF *createInstance(const std::string &deviceSpecification) {
			return new ChipATMega328P(deviceSpecification);
		}
		static string getFactoryNameReference() {
			return "ATMEGA328P";
		}
		~ChipATMega328P() {};
		
	protected:
		ChipATMega328P(const std::string &deviceSpecification) : ChipATMega328("P") {};
        
	private:
    	ChipATMega328P& operator=(const ChipATMega328P& rhs);
};


#endif /* CHIPATMEGA328P_HH_ */
