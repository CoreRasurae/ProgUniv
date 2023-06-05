/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ATMELGenericParallelIF.hh
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
 
#ifndef _ATMEL_GENERIC_PARALLEL_IF__
#define _ATMEL_GENERIC_PARALLEL_IF__
 

namespace InterfaceClass
{
	/**
	 * Interface for the ATMELGenericParallelIF class
	 * Defines the methods that must be made available by the implementation class.
	 * The methods provided are an abstraction to access and control the chip programming
	 * pin's for the parallel programming protocol. 
	 */
	 class ATMELGenericParallelIF
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
	 		 static const int CHIP_HIGH_VOLTAGE = 2;
	 	
	 		/**
	 		 * Set the OutputEnable state (inverted signal)
	 		 * @param value 0 to activate the OutputEnable signal
	 		 *              1 to disable the OutputEnable
	 		 */
	 		void set_OE(unsigned char value);
	 		
	 		/**
	 		 * Set the Write signal state (inverted signal)
	 		 * @param value 0 for a Write
	 		 *              1 for a Read
	 		 */
	 		void set_WR(unsigned char value);
	 		
	 		/**
	 		 * Set ByteSelect1 signal state (non-inverted signal)
	 		 * @param value 0 or 1 to clear or set the BS1 bit, respectively
	 		 */
	 		void setBS1(unsigned char value);
	 		
	 		/**
	 		 * Set ByteSelect2 signal state (non-inverted signal)
	 		 * @param value 0 or 1 to clear or set the BS2 bit, respectively
	 		 */
	 		void setBS2(unsigned char value);
	 		
	 		/**
	 		 * Set XTAL Action Bit 0 signal state (non-inverted signal)
	 		 * @param value 0 or 1 to clear or set the XA0 bit, respectively
	 		 */
	 		void setXA0(unsigned char value);
	 		
	 		/**
	 		 * Set XTAL Action Bit 1 signal state (non-inverted signal)
	 		 * @param value 0 or 1 to clear or set the XA1 bit, respectively
	 		 */
	 		void setXA1(unsigned char value);
	 		
	 		/**
	 		 * Set PAGE Load signal state
	 		 * @param value 0 or 1 to clear or set de PAGEL bit, repectively
	 		 */
	 		void setPAGEL(unsigned char value);
	 		
	 		/**
	 		 * Send's a byte to the chip DATA bus
	 		 * @param byte the byte to send
	 		 */
	 		void setDATA(unsigned char byte);
	 		
	 		/**
	 		 * Read's a byte from the chip DATA bus
	 		 * @return the byte read from the chip DATA bus
	 		 */
	 		unsigned char getDATA();
	 		
	 		/**
	 		 * Get the READY/BUSY state of the chip
	 		 * @return true, if the chip is READY
	 		 *         false, if the chip is BUSY
	 		 */
	 		bool getRDY_BSY();
	 		
	 		/**
	 		 * Set the CHIP power state
	 		 * @param value CHIP_POWER_ON, the chip supply power is turned on
	 		 *              CHIP_POWER_OFF, the chip supply power is removed
	 		 */
	 		void setPOWER(unsigned char value);
	 		
	 		/**
	 		 * Set the CHIP reset/program state
	 		 * @param value CHIP_RESET, put's the chip in RESET state
	 		 *              CHIP_OPERATION, put's the chip in OPERATION mode (+5V)
	 		 *              CHIP_HIGH_VOLTAGE, put's the chip in PROGRAM mode (+12V)
	 		 **/
	 		void set_RESET(unsigned char value);
	 		
	 	protected:    
        	ATMELGenericParallelIF()   {}
        	ATMELGenericParallelIF(const ATMELGenericParallelIF& rhs) {}
    	private:
        	ATMELGenericParallelIF& operator=(const ATMELGenericParallelIF& rhs);
	 }
}
 
#endif /* _ATMEL_GENERIC_PARALLEL_IF__ */