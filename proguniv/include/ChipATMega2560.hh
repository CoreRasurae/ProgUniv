/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ChipATMega2560.hh
 *
 *  Copyright  2022  Lu�s Mendes
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

#ifndef CHIPATMEGA2560_HH_
#define CHIPATMEGA2560_HH_

#include "../include/ChipIF.hh"
#include "../include/Chip.hh"
#include "../include/Property.hh"

/**
 * The implementation with the ATMEL AVR ChipATMega2560 characteristics
 * Abstract representation of the chip memories and configuration parameters 
 */
class ChipATMega2560 : public Chip {
	public:
		static ChipIF *createInstance(const std::string &deviceSpecification);
		static string getFactoryNameReference();
		~ChipATMega2560();
		virtual void setExternalClockFrequency(unsigned long frequency);
		virtual unsigned long getExternalClockFrequency();
		virtual unsigned long getInternalClockFrequency();
		
	protected:
      		unsigned long externalClockFrequency;
		unsigned long internalClockFrequency;
		
		ChipATMega2560(const std::string &deviceSpecification);
        
		virtual bool compareWithFile(const char* filename);
		virtual bool compareWithChip(ChipIF& otherChip);
		virtual void setInternalClockFrequency(unsigned long newInternalClockFrequency);

	private:
    	ChipATMega2560& operator=(const ChipATMega2560& rhs);
};


#endif /* CHIPATMEGA644_HH_ */
