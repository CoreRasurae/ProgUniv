/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ChipATTinyx5.hh
 *
 *  Copyright  2007  Lu�s Mendes
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

#ifndef CHIPATTINYX5_HH_
#define CHIPATTINYX5_HH_

#include "../include/ChipIF.hh"
#include "../include/Chip.hh"

/**
 * The implementation with the ATMEL AVR ATMegaxx8 characteristics
 * Abstract representation of the chip memories and configuration parameters 
 */
class ChipATTinyx5 : public Chip {
	public:
		~ChipATTinyx5();
		
	protected:
		ChipATTinyx5();
        
		virtual bool compareWithFile(const char* filename);
		virtual bool compareWithChip(ChipIF& otherChip);

	private:
    	ChipATTinyx5& operator=(const ChipATTinyx5& rhs);
};


#endif /* CHIPATTINYX5_HH_ */
