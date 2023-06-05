/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            Programmer.hh
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


#ifndef _PROGRAMMER_HH_
#define _PROGRAMMER_HH_

#include <string>
#include "../include/ChipIF.hh"
#include "../include/InvalidAddressException.hh"
#include "../include/ProgrammingErrorException.hh"
#include "../include/ReadingErrorException.hh"
#include "../include/EraseErrorException.hh"
#include "../include/InvalidParameterValueException.hh"
#include "../include/UnknownParameterNameException.hh"

/**
 * Programmer class, virtual class that abstracts the common Programmer hardware operations and elements.
 */
class Programmer
{
	public:
		/**
		 * Creates a new Programmer instance
		 * @param prog_name the specific programmer name
		 * @param manufacturer_name the chip manufacturer name
		 * @param chip_ref the chip reference identifier
		 * @return a new programmer instance
		 *
		 * The prog_name field maybe usefull if the same class can represent two or more different programmers
		 */
		shared_ptr<Programmer> create(const char *prog_name, const char *manufacturer_name, const char *chip_ref); //{ return 0; }

		/**
		 * If the programmer is connected to an application board and if the board has its own clock source
		 * even during the programming cycle, then its frequency should be identified, as a more optimized
		 * algorithm may be used depending on the board clock frequency.
		 * @param the target board frequency in Hz
		 */
		virtual void setTargetBoardClockFrequency(unsigned long frequency) {
			this->frequency = frequency;
			if (getChip() != 0) {
				getChip()->setExternalClockFrequency(frequency);
			}
		}

		/**
		 * Retrieves the clock frequency of the target board. This frequency indicates that the target board, that is, the board
		 * that contains the device to be programmed is clocking the device at a given frequency. This may enable the programmer
		 * board to choose the best tactic/algorithm to program the device.
		 * @return the target board clock frequency value in Hz, or 0 if device is not being clocked
		 */
		virtual unsigned long getTargetBoardClockFrequency() {
			return frequency;
		}

		/**
		 * Set's a programmer configuration parameter
		 * @param parameter the parameter name identifier string
		 * @param value the value string
		 */
		virtual void setParameter(const std::string &parameter, const std::string &value) = 0;

		/**
		 * Checks if the uC conntected to programmer hardware if of the expected type,
		 * that is, if it matches the type of the virtual that has been associated to
		 * the programmer.
		 * @return	true, if the signature matches
		 *          false, otherwise
		 */
		virtual bool checkChipSignature(bool *isLocked = 0) = 0;

		/**
		 * Program the specified memory of the real uC chip to match the contents
		 * of the virtual uC.
		 * @param memoryId identifier of the memory to program,
		 *                 "ALL" to program all defined memories for the current uC
		 * @param verifyWhileWrite when set to true, will cause all written memory positions to be verified
		 * @param isChipErased
		 * 			<ul><li>when set to true, all memory positions of the
		 * 			memory or memories identified by memoryId will be
		 * 			considered as being fully erased, as such all empty
		 * 			locations in the source memory will be skipped</li>
		 * 			<li>when set to false, all memory positions will be
		 * 			verified, disregarding if the source location is
		 * 			empty or not</li></ul>
		 * @throw InvalidAddressException if an access outside of the chip memory address space has been detected
		 * @throw ProgrammingErrorException if a programming error occurred
		 */
		virtual void writeMemory(const std::string &memoryId, const bool verifyWhileWrite, const bool isChipErased) = 0;

		/**
		 * Read the specified memory of the real uC chip into the memory of the virtual
		 * uC representation.
		 * @param memoryId identifier of the memory to read,
		 *                 "ALL" to program all defined memories for the current uC
		 * @throw InvalidAddressException if an access outside of the chip memory address space has been detected
		 * @throw ReadingErrorException if a reading error occurred
		 */
		virtual void readMemory(const std::string &memoryId) = 0;

		/**
		 * Erases the specified memory of the real uC chip
		 * @param memoryId identified of the memory to erase,
		 *                 "ALL" to erase all defined memoried for the current uC
		 */
		virtual void eraseMemory(const std::string &memoryId) = 0;

		/**
		 * Writes configuration data into the real uC chip
		 */
		virtual void writeConfiguration() = 0;

		/**
		 * Reads the current real uC chip configuration into the virtual uC chip
		 * representation
		 */
		virtual void readConfiguration() = 0;

		/**
		 * Erase current real uC chip configuration memory
		 */
		virtual void eraseConfiguration() = 0;

		/**
		 * Erase all memories (assumes that all memories are protected)
		 */
		virtual void eraseAll() = 0;

		/**
		 * Get the chip associated with the programmer
		 * @return a pointer to the chip instance
		 */
		virtual InterfaceClass::ChipIF *getChip() = 0;

		/**
		 * Get the last operation status code
		 * @return the last operation status code value
		 */
		virtual int getOperationResult() { return 0; }

		virtual ~Programmer() {}

	protected:
		bool init;
		bool in_program_mode;
		char *nome_prog;
		unsigned long frequency;

		Programmer() {
			frequency = 0;
		}
};

#endif //_PROGRAMMER_HH_
