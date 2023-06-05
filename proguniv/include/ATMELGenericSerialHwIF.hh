/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ATMELGenericSerialHwIF.hh
 *
 *  Copyright  2007  Luís Mendes
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
 
#ifndef ATMEL_GENERIC_SERIAL_HW_IF_HH_
#define ATMEL_GENERIC_SERIAL_HW_IF_HH_

namespace InterfaceClass
{
	/**
	 * Interface ATMELGenericSerialHwIF class
	 * Defines the methods that must be made available by the implementation class.
	 * The methods provided are an abstraction to access and control the chip programming
	 * pin's for the serial programming protocol. 
	 */
	 class ATMELGenericSerialHwIF
	 {
	 	public:
	 		/**
	 		 * CHIP POWER constant's 
	 		 */
	 		static const int CHIP_POWER_ON = 1;
	 		static const int CHIP_POWER_OFF = 0;
	 		 
	 		/**
	 		 * CHIP RESET contant's
	 		 */
	 		static const int CHIP_RESET = 0;
	 		static const int CHIP_OPERATION = 1;
	 		
	 		virtual ~ATMELGenericSerialHwIF() {}
	 		
	 		/**
	 		 * Set the Master Output Slave In (MOSI) serial data in state
	 		 * @param value 0, for bit 0
	 		 *              1, for bit 1
	 		 */
	 		virtual void setMOSI(unsigned char value) = 0;
	 		
	 		/**
	 		 * Read the Master Input Slave Output (MISO) serial data out state
	 		 * @return 0, for bit 0
	 		 *         1, for bit 1
	 		 */
	 		virtual unsigned char getMISO() = 0;
	 		
	 		/**
	 		 * Set the Serial Clock (SCK) state
	 		 * @param value 0, for clock LOW
	 		 *              1, for clock HIGH
	 		 */ 
	 		virtual void setSCK(unsigned char value) = 0;
	 		
	 		
	 		/**
	 		 * Set the CHIP power state
	 		 * @param value CHIP_POWER_ON, the chip supply power is turned on
	 		 *              CHIP_POWER_OFF, the chip supply power is removed
	 		 */
	 		virtual void setPOWER(unsigned char value) = 0;
	 		
	 		/**
	 		 * Set the CHIP reset/program state
	 		 * @param value CHIP_RESET, put's the chip in RESET state
	 		 *              CHIP_OPERATION, put's the chip in OPERATION mode (+5V)
	 		 **/
	 		virtual void setRESET(unsigned char value) = 0;
	 		
	 		/**
	 		 * Control the CHIP clock line state
	 		 * @param value 0, for clock LOW
	 		 *              1, for clock HIGH
	 		 */
	 		 virtual void setCLK(unsigned char value) = 0;
	 		
	 	protected:
			ATMELGenericSerialHwIF() {}
	        ATMELGenericSerialHwIF(const ATMELGenericSerialHwIF& rhs) {}

    	private:
        	ATMELGenericSerialHwIF& operator=(const ATMELGenericSerialHwIF& rhs);
	 };
}

#endif /* ATMEL_GENERIC_SERIAL_HW_IF_HH_ */
