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

#ifndef CHIPATMEGA88PA_HH_
#define CHIPATMEGA88PA_HH_

#include "../include/ChipIF.hh"
#include "../include/ChipATMega88.hh"

/**
 * The implementation with the ATMEL AVR ATMega88PA characteristics
 * Abstract representation of the chip memories and configuration parameters 
 */
class ChipATMega88PA : public ChipATMega88 {
	public:
		static ChipIF *createInstance(const std::string &deviceSpecification) {
			return new ChipATMega88PA(deviceSpecification);
		}
		static string getFactoryNameReference() {
			return "ATMEGA88PA";
		}
		~ChipATMega88PA() {};
		
	protected:
		ChipATMega88PA(const std::string &deviceSpecification) : ChipATMega88("PA") {};
        
	private:
    	ChipATMega88PA& operator=(const ChipATMega88PA& rhs);
};


#endif /* CHIPATMEGA88PA_HH_ */
