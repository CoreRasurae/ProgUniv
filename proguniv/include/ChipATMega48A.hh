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

#ifndef CHIPATMEGA48A_HH_
#define CHIPATMEGA48A_HH_

#include "../include/ChipIF.hh"
#include "../include/ChipATMega48.hh"

/**
 * The implementation with the ATMEL AVR ATMega48A characteristics
 * Abstract representation of the chip memories and configuration parameters 
 */
class ChipATMega48A : public ChipATMega48 {
	public:
		static ChipIF *createInstance(const std::string &deviceSpecification) {
			return new ChipATMega48A(deviceSpecification);
		}
		static string getFactoryNameReference() {
			return "ATMEGA48A";
		}
		~ChipATMega48A() {};
		
	protected:
		ChipATMega48A(const std::string &deviceSpecification) : ChipATMega48("A") {};
        
	private:
    	ChipATMega48A& operator=(const ChipATMega48A& rhs);
};


#endif /* CHIPATMEGA48A_HH_ */
