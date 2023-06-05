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

#ifndef CHIPATMEGA168PA_HH_
#define CHIPATMEGA168PA_HH_

#include "../include/ChipIF.hh"
#include "../include/ChipATMega168.hh"

/**
 * The implementation with the ATMEL AVR ATMega168A characteristics
 * Abstract representation of the chip memories and configuration parameters 
 */
class ChipATMega168PA : public ChipATMega168 {
	public:
		static ChipIF *createInstance(const std::string &deviceSpecification) {
			return new ChipATMega168PA(deviceSpecification);
		}
		static string getFactoryNameReference() {
			return "ATMEGA168PA";
		}
		~ChipATMega168PA() {};
		
	protected:
		ChipATMega168PA(const std::string &deviceSpecification) : ChipATMega168("PA") {};
        
	private:
    	ChipATMega168PA& operator=(const ChipATMega168PA& rhs);
};


#endif /* CHIPATMEGA168PA_HH_ */
