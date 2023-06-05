/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ATMELGenericSerialPagedMem.hh
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

#include <memory>

#ifndef ATMELGENERICSERIALPAGEDMEM_HH_
#define ATMELGENERICSERIALPAGEDMEM_HH_

#include "../include/ATMELGenericSerial.hh"

/**
 * This class implements the serial programming protocol for
 * ATMEL AVR Paged Memory Devices
 */
class ATMELGenericSerialPagedMem : public ATMELGenericSerial {
public:
	ATMELGenericSerialPagedMem(std::shared_ptr<InterfaceClass::ATMELGenericSerialHwIF> realProgrammer, std::shared_ptr<InterfaceClass::ChipIF> targetChip);
	virtual ~ATMELGenericSerialPagedMem();
	
protected:
	virtual void writeConfiguration();
	
	virtual void readConfiguration();
	
	/**
	 * Internal helper method to program paged EEPROM and paged Program memories
	 * @param verifyWhileWrite true, if the data written should be checked while programming
	 *                         false, to program without verfifying
	 * @param memoryId must be "Program" to program paged Program memories,
	 *                   or "Data" to program paged EEPROM memories
	 * @param useTestData true, if the memory is to be filled with a test pattern
	 *                    false, to use real data
	 * @throw NotInProgrammingModeException if the chip is not in programming mode
	 * @throw InvalidAddressException if an access is made outside the current memory space address
	 * @throw ProgrammingErrorException if an error occurs during chip programming
	 */
	virtual void writeMemoryInternal(const bool verifyWhileWrite, const bool isChipErased, const std::string &memoryId, const bool useTestData);
};

#endif /*ATMELGENERICSERIALPAGEDMEM_HH_*/
