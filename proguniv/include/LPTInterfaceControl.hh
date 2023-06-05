/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            LPTInterfaceControl.hh
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

#ifndef _LPTINTERFACECONTROL_HH_
#define _LPTINTERFACECONTROL_HH_

#include "../include/LPTLowLevelInterface.hh"
#include "../include/InvalidLPTPinException.hh"
#include "../include/InvalidLPTBaseAddressException.hh"

class LPTInterfaceControl
{
public:
	typedef enum LPTPinEnum { lptStatusnACK, lptStatus_Busy, lptStatusPaperOut, lptStatusSelect, lptStatusnError, 
							  lptControl_nStrobe, lptControl_nAutoLF, lptControlnINIT, lptControl_nSelect } LPTPin;

	static LPTInterfaceControl& getInstance(int address);
	
	/**
	 * Writes a data byte to the parallel port
	 * @param byte the data byte to send
	 */
	void sendByte(unsigned char byte);
	
	/**
	 * Reads a data byte from the parallel port
	 * @return the data byte read
	 */
	unsigned char readByte();
	
	/**
	 * Set's/Clears the LPT output Latch into/from Tri-state, enabling data to be read ou written
	 * @param rwMode true, to put the output latch into tri-state
	 *               false, to activate the output latch
	 */
	void setRWMode(bool rwMode); 
	
	/**
	 * Set's a single data bit pin state
	 *  @param nbit a value in the range 0 to 7 inclusive, the bit number
	 *  @param bit_state 0, for LOW level
	 *                   1, for HIGH level
	 */ 
	void setDataBit(int nbit, unsigned char bit_state);
	
	/**
	 * Get's the state of a single data bit pin
	 * @param nbit a value in the range o to 7 inclusive, the bit number
	 * @return 0, for LOW level
	 *         1, for HIGH level
	 */
	unsigned char readDataBit(int nbit);
	
	/**
	 * Reads the acknowledge bit pin
	 * @return 0, for LOW level
	 *         1, for HIGH level
	 */
	unsigned char readnACK();
	
	/**
	 * Read's the LPT input pin with the specified identifier
	 * @param pinId the pin identifier
	 */
	unsigned char readLPTPin(LPTPin pinId);
	
	/**
	 * Set's a control PIN state
	 * @param pin the pin identifier
	 * @param bit_state 0, for LOW level
	 *                  1, for HIGH level
	 */
	void writeLPTPin(const LPTPin pinId, unsigned char bit_state);
	
private:
	unique_ptr<InterfaceClass::LPTLowLevelInterface> lptInterface;

	int base_lpt;
	/**
	 * If set to true, will cause the lpt output port state to be cached on
	 * first access and further output port accesses will used the cached
	 * value in order to increase I/O performance (typically is 2x faster).
	 * If set to false, the value of lpt output port state will be read every
	 * single access, this has poorer I/O performance, but gives more protection
	 * against I/O access outside this API or for software bugs.
	 */
	bool store_lpt_state;
	/**
	 * Flag that indicates if the cached_output_port_value variable has an
	 * usable value.
	 */
	bool output_port_cache_is_valid;
	/**
	 * Cache of the lpt output port value for an LPTInterfaceControl instance
	 * There will be one value for each LPT port used (inside the object instance)
	 */
	unsigned char cached_output_port_value;
	
	static LPTInterfaceControl *instance[]; //Reserve space for up to three LPT ports

	LPTInterfaceControl();
	~LPTInterfaceControl();
	LPTInterfaceControl(LPTInterfaceControl const&); //Esconder construtor por cópia
	LPTInterfaceControl& operator= (LPTInterfaceControl const&); //Operador de atribuíção escondido
	
	/**
	 * Initializes an LPTInterfaceControl instance to a given address
	 * @param address the LPT address that the invoking instance should use
	 */
	void initLPTInterfaceControl(int address);
	
	/**
	 * Decode an LPTPin id into an LPT base address offset and bit number
	 * @param pinId the LPT pinId to decode
	 * @param offset variable which will store the pin register address offset
	 * @param bit variable which will store the bit offset within the above specified register
	 */ 
	void decodeLPTPin(const LPTPin pinId, int& offset, int& bit);
};

#endif //_LPTINTERFACECONTROL_HH_
