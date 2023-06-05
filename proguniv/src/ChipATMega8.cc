/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            ChipATMega8.cc
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
#include "../include/Utils.hh"
#include "../include/Memoria.hh"
#include "../include/ChipIF.hh"
#include "../include/Property.hh"
#include "../include/ChipATMega8.hh"
#include "../include/MemoryInterceptorProxy.hh"

namespace ATMega8 {

	//LOCK Bit Byte
    void updateLockBit(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Lock Bit Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Set unused bits
		old_value |= (0xc0);

		//Clear the bits
		old_value &= ~(0x03);

		if ( value.compare("Mode1") == 0 ) {
			//Set bits 1,0 = "11"
			old_value |= 0x03;
		}
		else if ( value.compare("Mode2") == 0 ) {
			//Set bits 1,0 = "10"
			old_value |= 0x02;
		}
		else if ( value.compare("Mode3") == 0 ) {
			//Set bits 1,0 = "00"
			old_value |= 0x00;
		}
		else {
			//Undocumented, bits 1,0 = "01"
			old_value |= 0x01;
		}

		mem.escreve(old_value);
		cout << "update LB: " << old_value << endl;
	}

	std::string reverseUpdateLockBit(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Lock Bit Byte");

		mem.le(&value);

		//Extract useful bits
		value &= 0x03;

		cout << "reverse update LB: " << value << endl;
		if ( value == 3 ) {
			return "Mode1";
		}
		else if ( value == 2 ) {
			return "Mode2";
		}
		else if ( value == 0 ) {
			return "Mode3";
		}
		else {
			return "Undocumented";
		}
	}

	void updateBootLockBit0(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Lock Bit Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Set the default bits
		old_value |= (0x03 << 6);

		//Clear the bits
		old_value &= ~(0x03 << 2);

		if ( !value.compare("Mode1") ) {
			//Set bits 3,2 = "11"
			old_value |= (0x03 << 2);
		}
		else if ( !value.compare("Mode2") ) {
			//Set bits 3,2 = "10"
			old_value |= (0x02 << 2);
		}
		else if ( !value.compare("Mode3") ) {
			//Set bits 3,2 = "00"
		}
		else if ( !value.compare("Mode4") ) {
			//Set bits 3,2 = "01"
			old_value |= (0x01 << 2);
		}

		mem.escreve(old_value);
		cout << "update BLB0: " << old_value << endl;
	}

	std::string reverseUpdateBootLockBit0(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Lock Bit Byte");

		mem.le(&value);
		//Get bits 2 and 3
		value >>= 2;
		value &= 0x03;

		cout << "reverse update BLB0: " << value << endl;

		if ( value == 3 ) {
			return "Mode1";
		}
		else if ( value == 2 ) {
			return "Mode2";
		}
		else if ( value == 0 ) {
			return "Mode3";
		}
		else {
			//Value == 1
			return "Mode4";
		}
	}

	void updateBootLockBit1(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Lock Bit Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Set the default bits
		old_value |= (0x03 << 6);

		//Clear the bits
		old_value &= ~(0x03 << 4);

		if ( value.compare("Mode1") == 0 ) {
			//Set bits 5,4 = "11"
			old_value |= (0x03 << 4);
		}
		else if ( value.compare("Mode2") == 0 ) {
			//Set bits 5,4 = "10"
			old_value |= (0x02 << 4);
		}
		else if ( value.compare("Mode3") == 0 ) {
			//Set bits 5,4 = "00"
		}
		else if ( value.compare("Mode4") == 0 ) {
			//Set bits 5,4 = "01"
			old_value |= (0x01 << 4);
		}

		mem.escreve(old_value);
		cout << "update BLB1: " << old_value << endl;
	}

	std::string reverseUpdateBootLockBit1(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Lock Bit Byte");

		mem.le(&value);
		//Get bits 4 and 5
		value >>= 4;
		value &= 0x03;

		cout << "reverse update BLB1: " << value << endl;

		if ( value == 3 ) {
			return "Mode1";
		}
		else if ( value == 2 ) {
			return "Mode2";
		}
		else if ( value == 0 ) {
			return "Mode3";
		}
		else {
			//Value == 1
			return "Mode4";
		}
	}

	//Handle Fuse High Byte
	void updateFuseHighRSTDISBL(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(1 << 7);

		if ( value.compare("Disabled") == 0 ) {
			mem.escreve( old_value | (1 << 7) );
		}
		else {
			//Enabled
			mem.escreve( old_value );
		}
	}

	std::string reverseUpdateFuseHighRSTDISBL(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");

		mem.le(&value);

		value >>= 7;
		value &= 0x01;

		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
	}

	void updateFuseHighWDTON(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(1 << 6);

		if ( value.compare("Disabled") == 0 ) {
			mem.escreve( old_value | (1 << 6) );
		}
		else {
			//Enabled
			mem.escreve( old_value );
		}
	}

	std::string reverseUpdateFuseHighWDTON(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");

		mem.le(&value);

		value >>= 6;
		value &= 0x01;

		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
	}

	void updateFuseHighSPIEN(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(1 << 5);

		if ( value.compare("Disabled") == 0 ) {
			mem.escreve( old_value | (1 << 5) );
		}
		else {
			//Enabled
			mem.escreve( old_value );
		}
	}

	std::string reverseUpdateFuseHighSPIEN(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");

		mem.le(&value);

		value >>= 5;
		value &= 0x01;

		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
	}

	void updateFuseHighCKOPT(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(1 << 4);

		if ( value.compare("High") == 0 ) {
			mem.escreve( old_value | (1 << 4) );
		}
		else {
			//Enabled
			mem.escreve( old_value );
		}
	}

	std::string reverseUpdateFuseHighCKOPT(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");

		mem.le(&value);

		value >>= 4;
		value &= 0x01;

		if ( value == 0x01 ) {
			return "High";
		}
		else
			return "Low";
	}


	void updateFuseHighEESAVE(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(1 << 3);

		if ( value.compare("Disabled") == 0 ) {
			mem.escreve( old_value | (1 << 3) );
		}
		else {
			//Enabled
			mem.escreve( old_value );
		}
	}

	std::string reverseUpdateFuseHighEESAVE(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");

		mem.le(&value);

		value >>= 3;
		value &= 0x01;

		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
	}

	void updateFuseHighBOOTSZ(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(0x03 << 1);

		if ( value.compare("128w") == 0 ) {
			mem.escreve( old_value | (0x03 < 1));
		}
		else if ( value.compare("256w") == 0 ) {
			mem.escreve( old_value | (0x02 < 1));
		}
		else if ( value.compare("512w") == 0 ) {
			mem.escreve( old_value | (0x01 < 1));
		}
		else {
			//1024w
			mem.escreve( old_value );
		}
	}

	std::string reverseUpdateFuseHighBOOTSZ(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");

		mem.le(&value);
		value >>= 1;
		value &= 0x03;

		if (value == 0x03) {
			return "128w";
		}
		else if (value == 0x02) {
			return "256w";
		}
		else if (value == 0x01) {
			return "512w";
		}
		else {
			return "1024w";
		}
	}

	void updateFuseHighBOOTRST(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~0x01;

		if ( value.compare("Vector0000") == 0 ) {
			mem.escreve( old_value | 0x01 );
		}
		else {
			//BootVector
			mem.escreve( old_value );
		}
	}

	std::string reverseUpdateFuseHighBOOTRST(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");

		mem.le(&value);

		value &= 0x01;

		if ( value == 0x01 ) {
			return "Vector0000";
		}
		else
			return "BootVector";
	}

	//Handle Fuse Low Byte
	void updateFuseLowBodLevel(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse Low Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(0xC0);

		if ( value.compare("2V7") == 0 ) {
			//bits 7,6 = 10
			mem.escreve( old_value | 0x80 );
		} else if ( value.compare("4V0") == 0 ) {
			//bits 7,6 = 00
			mem.escreve( old_value );
		} else {
			//bits 7,6 = x0 -> Disabled
			mem.escreve(old_value | 0x40);
		}
	}

	std::string reverseUpdateFuseLowBodLevel(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse Low Byte");

		mem.le(&value);
		value >>= 6;
		value &= 0x03;

		if ( value == 0x02 ) {
			return "2V7";
		}
		else if (value == 0x00){
			return "4V0";
		} else {
			return "Disabled";
		}
	}

	void updateFuseLowSUT(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse Low Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(0x03 << 4);

		if ( value.compare("11") == 0 ) {
			mem.escreve( old_value | (0x03 << 4) );
		}
		else if ( value.compare("10") == 0 ) {
			mem.escreve( old_value | (0x02 << 4) );
		}
		else if ( value.compare("01") == 0 ) {
			mem.escreve( old_value | (0x01 << 4) );
		}
		else {
			//00
			mem.escreve( old_value );
		}
	}

	std::string reverseUpdateFuseLowSUT(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse Low Byte");

		mem.le(&value);

		value >>= 4;
		value &= 0x03;

		if ( value == 0x03 ) {
			return "11";
		}
		else if ( value == 0x02 ) {
			return "10";
		}
		else if ( value == 0x01 ) {
			return "01";
		}
		else {
			return "00";
		}
	}

	void updateFuseLowCKSEL(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria &mem = *chip.getMemory("Fuse Low Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(0x0f);

		//We have to bypass the MemoryInterceptorProxy (if set), because
		//the reverseUpdate should only be called when reading from the
		//chip. The changes made here haven't got into the chip yet.
		//This avoids an unnecessary call to the reverse update and
		//avoids breaking the programmer, by thinking that the internal
		//clock source is already configured.
		if ( value.compare("1111") == 0 ) {
			mem.Memoria::escreve( old_value | 0x0f );
		}
		else if ( value.compare("1110") == 0 ) {
			mem.Memoria::escreve( old_value | 0x0e );
		}
		else if ( value.compare("1101") == 0 ) {
			mem.Memoria::escreve( old_value | 0x0d );
		}
		else if ( value.compare("1100") == 0 ) {
			mem.Memoria::escreve( old_value | 0x0c );
		}
		else if ( value.compare("1011") == 0 ) {
			mem.Memoria::escreve( old_value | 0x0b );
		}
		else if ( value.compare("1010") == 0 ) {
			mem.Memoria::escreve( old_value | 0x0a );
		}
		else if ( value.compare("1001") == 0 ) {
			mem.Memoria::escreve( old_value | 0x09 );
		}
		else if ( value.compare("1000") == 0 ) {
			mem.Memoria::escreve( old_value | 0x08 );
		}
		else if ( value.compare("0111") == 0 ) {
			mem.Memoria::escreve( old_value | 0x07 );
		}
		else if ( value.compare("0110") == 0 ) {
			mem.Memoria::escreve( old_value | 0x06 );
		}
		else if ( value.compare("0101") == 0 ) {
			mem.Memoria::escreve( old_value | 0x05 );
		}
		else if ( value.compare("0100") == 0 ) {
			mem.Memoria::escreve( old_value | 0x04 );
		}
		else if ( value.compare("0011") == 0 ) {
			mem.Memoria::escreve( old_value | 0x03 );
		}
		else if ( value.compare("0010") == 0 ) {
			mem.Memoria::escreve( old_value | 0x02 );
		}
		else if ( value.compare("0001") == 0 ) {
			mem.Memoria::escreve( old_value | 0x01 );
		}
		else {
			//0000
			mem.Memoria::escreve( old_value );
		}
	}

	std::string reverseUpdateFuseLowCKSEL(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse Low Byte");

		mem.le(&value);

		value &= 0x0f;

		if ( value == 0x0f ) {
			return "1111";
		}
		else if ( value == 0x0e ) {
			return "1110";
		}
		else if ( value == 0x0d ) {
			return "1101";
		}
		else if ( value == 0x0c ) {
			return "1100";
		}
		else if ( value == 0x0b ) {
			return "1011";
		}
		else if ( value == 0x0a ) {
			return "1010";
		}
		else if ( value == 0x09 ) {
			return "1001";
		}
		else if ( value == 0x08 ) {
			return "1000";
		}
		else if ( value == 0x07 ) {
			return "0111";
		}
		else if ( value == 0x06 ) {
			return "0110";
		}
		else if ( value == 0x05 ) {
			return "0101";
		}
		else if ( value == 0x04 ) {
			return "0100";
		}
		else if ( value == 0x03 ) {
			return "0011";
		}
		else if ( value == 0x02 ) {
			return "0010";
		}
		else if ( value == 0x01 ) {
			return "0001";
		}
		else {
			return "0000";
		}
	}

	//Update internal clock source property value
    std::string reverseUpdateInternalClockSource(InterfaceClass::ChipIF& chip) {
    	//This chip has four internal clock sources one of aprox. 1MHz
    	//and another of aprox. 2MHz, 4MHz and 8Mhz.

    	//0001 - 1.0MHz calibrated internal clock source
    	//0010 - 2.0MHz calibrated internal clock source
    	//0011 - 4.0MHz calibrated internal clock source
    	//0100 - 8.0MHz calibrated internal clock source
    	std::string result = reverseUpdateFuseLowCKSEL(chip);
    	if ( result.compare("0100") == 0 ) {
    		chip.setInternalClockFrequency(8000000);
    		return "true";
    	} else if (result.compare("0011") == 0) {
    		chip.setInternalClockFrequency(4000000);
    		return "true";
    	} else if (result.compare("0010") == 0) {
    		chip.setInternalClockFrequency(2000000);
    		return "true";
    	} else if (result.compare("0001") == 0) {
    		chip.setInternalClockFrequency(1000000);
    		return "true";
    	} else {
    		return "false";
    	}
    }
}

ChipATMega8::ChipATMega8(const std::string &deviceSpecification) {
	using namespace ATMega8;

	externalClockFrequency = 0;
	internalClockFrequency = 0;

	//Add chip memories
	shared_ptr<MemoryInterceptorProxy> lock_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Lock Bit Byte"));
	this->addMemory( lock_memPtr->obtem_nome(), static_pointer_cast<Memoria,MemoryInterceptorProxy>(lock_memPtr) );

	shared_ptr<MemoryInterceptorProxy> high_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Fuse High Byte"));
	this->addMemory( high_memPtr->obtem_nome(), static_pointer_cast<Memoria,MemoryInterceptorProxy>(high_memPtr) );

	shared_ptr<MemoryInterceptorProxy> low_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Fuse Low Byte"));
	this->addMemory( low_memPtr->obtem_nome(), static_pointer_cast<Memoria,MemoryInterceptorProxy>(low_memPtr) );

	shared_ptr<Memoria> memPtr = shared_ptr<Memoria>(new Memoria(8, "Calibration Byte"));
	this->addMemory( memPtr->obtem_nome(), memPtr );

	memPtr =  shared_ptr<Memoria>(new Memoria(4096, 0x0000, 16, "Program"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xffff, 0x0000, 4096);

	memPtr = shared_ptr<Memoria>(new Memoria(512, 0x0000, 8, "Data"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xff, 0x0000, 512);

	/********************************************
	 * Programmer Properties
	 ********************************************/
	//Add programmer related specific chip properties
	shared_ptr<Property> progPropPtr = shared_ptr<Property>(new Property( this, "MemoryType", "Paged", "Tells whether this chip contains paged memories or not", true ));
	progPropPtr->addAcceptedValue( "Paged", "This chip memory is of the Paged type" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_flash", "5000", "The amount of time required to program the flash memory at minimum Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "5000", "5.0 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_eeprom", "9000", "The amount of time required to program the eeprom memory at minimum Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "9000", "9.0 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_erase", "18000", "The amount of time required to erase memory at minimum Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "18000", "9.0 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_fuse", "5500", "The amount of time required to program fuse memory at minimum Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "5500", "5.5 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "HasInternalClockSource", "true", "Whether this chip can run from an internal clock", true ));
	progPropPtr->addAcceptedValue( "true", "This chip can run from an internal clock source" );
	//progPropPtr->addAcceptedValue( "false", "This chip can only run from an external clock source" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockDelay", "1", "Minimum amount of time that each clock half-cycle must take", true ));
	progPropPtr->addAcceptedValue( "1", "1 us" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesSCK0", "32", "Clock cycles to generate per bit when SCK is 0", true ));
	progPropPtr->addAcceptedValue( "32", "16 cycles" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesWriteSCK1", "32", "Clock cycles to generate per bit when SCK is 1 on a Write", true ));
	progPropPtr->addAcceptedValue( "32", "16 cycles" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesReadSCK1", "32", "Clock cycles to generate per bit when SCK is 1 on a Read", true ));
	progPropPtr->addAcceptedValue( "32", "16 cycles" );
	this->addProgrammerProperty( progPropPtr );

	//Add programmer related specific chip properties
	progPropPtr = shared_ptr<Property>(new Property( this, "ProgramPageSizeWordsBits", "5", "The size in bits of each page for the program memory", true ));
	progPropPtr->addAcceptedValue( "5", "Program memory page size is 32 words" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "ProgramTotalPages", "128", "The total number of program memory pages in this chip", true ));
	progPropPtr->addAcceptedValue( "128", "The total number of program memory pages is 128" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "DataPageSizeWordsBits", "2", "The size in bits of each page for the data memory", true ));
	progPropPtr->addAcceptedValue( "2", "Data memory page size is 4 words" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "DataTotalPages", "128", "The total number of program memory pages in this chip", true ));
	progPropPtr->addAcceptedValue( "128", "The total number of data memory pages is 128" );
	this->addProgrammerProperty( progPropPtr );

	//Set ATMega88 chip signature string
	this->setSignatureString("1E9307");

	//Set ATMega88 chip name
	this->setChipName("ATMEL ATMega8");

	/********************************************
	 * Chip Configuration Properties
	 ********************************************/
	//Lock Bit Byte
	//Configuration Properties for the LOCK byte
	shared_ptr<Property> propPtr = shared_ptr<Property>(new Property( this, "LOCKBYTE_LOCKBIT", "Mode1", "Memory Lock Options", false ));
	propPtr->addAcceptedValue( "Mode1", "No memory lock features enabled." );
	propPtr->addAcceptedValue( "Mode2", "Further programming of the Flash and EEPROM is disabled in \
										Parallel and Serial Programming mode. The Fuse bits are \
										locked in both Serial and Parallel Programming mode." );
	propPtr->addAcceptedValue( "Mode3", "Further programming and verification of the Flash and EEPROM \
										is disabled in Parallel and Serial Programming mode. The Boot \
										Lock bits and Fuse bits are locked in both Serial and Parallel \
										Programming mode." );
	propPtr->addAcceptedValue( "Undocumented", "This configuration is undocumented by ATMEL (bits 1,0 \
										are set to values 01");
	propPtr->registerForwardUpdater( (forwardPtr)&updateLockBit );
	propPtr->registerReverseUpdater( &reverseUpdateLockBit );
	this->addProperty(propPtr);
	lock_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "LOCKBYTE_BOOTLOCKBIT0", "Mode1", "Memory Boot Lock Options #1", false ));
	propPtr->addAcceptedValue( "Mode1", "No restrictions for SPM or (E)LPM accessing the Application \
										section." );
	propPtr->addAcceptedValue( "Mode2", "SPM is not allowed to write to the Application section." );
	propPtr->addAcceptedValue( "Mode3", "SPM is not allowed to write to the Application section, and \
										(E)LPM executing from the Boot Loader section is not allowed to \
										read from the Application section. If Interrupt Vectors are placed \
										in the Boot Loader section, interrupts are disabled while \
      									executing from the Application section." );
	propPtr->addAcceptedValue( "Mode4", "(E)LPM executing from the Boot Loader section is not allowed to \
										read from the Application section. If Interrupt Vectors are placed \
										in the Boot Loader section, interrupts are disabled while \
										executing from the Application section." );
	propPtr->registerForwardUpdater( (forwardPtr)&updateBootLockBit0 );
	propPtr->registerReverseUpdater( &reverseUpdateBootLockBit0 );
	this->addProperty( propPtr );
	lock_memPtr->addMemoryListener( propPtr );

	propPtr = shared_ptr<Property>(new Property( this, "LOCKBYTE_BOOTLOCKBIT1", "Mode1", "Memory Boot Lock Options #2", false ));
	propPtr->addAcceptedValue( "Mode1", "No restrictions for SPM or (E)LPM accessing the Boot Loader \
      									section.");
	propPtr->addAcceptedValue( "Mode2", "SPM is not allowed to write to the Boot Loader section." );
	propPtr->addAcceptedValue( "Mode3", "SPM is not allowed to write to the Boot Loader section, and \
										(E)LPM executing from the Application section is not allowed to \
										read from the Boot Loader section. If Interrupt Vectors are \
										placed in the Application section, interrupts are disabled while \
      									executing from the Boot Loader section." );
	propPtr->addAcceptedValue( "Mode4", "(E)LPM executing from the Application section is not allowed to \
										read from the Boot Loader section. If Interrupt Vectors are \
										placed in the Application section, interrupts are disabled while \
										executing from the Boot Loader section." );
	propPtr->registerForwardUpdater( (forwardPtr)&updateBootLockBit1 );
	propPtr->registerReverseUpdater( &reverseUpdateBootLockBit1 );
	this->addProperty( propPtr );
	lock_memPtr->addMemoryListener( propPtr );


	//Fuse High Byte
	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_RSTDISBL", "Disabled", "External Reset disable", false ));
	propPtr->addAcceptedValue( "Enabled", "The external reset pin is disconnected" );
	propPtr->addAcceptedValue( "Disabled", "The external reset pin is in use" );
	propPtr->registerForwardUpdater( &updateFuseHighRSTDISBL );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighRSTDISBL );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_WDTON", "Disabled", "Watchdog Timer always on", false ));
	propPtr->addAcceptedValue( "Enabled", "Watchdog Timer is always on" );
	propPtr->addAcceptedValue( "Disabled", "Watchdog Timer is controlled by software" );
	propPtr->registerForwardUpdater( &updateFuseHighWDTON );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighWDTON );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_SPIEN", "Enabled", "Enable Serial Program and Data Downloading", false ));
	propPtr->addAcceptedValue( "Enabled", "Serial Program and Data Downloading enabled" );
	propPtr->addAcceptedValue( "Disabled", "Serial Program and Data Downloading disabled" );
	propPtr->registerForwardUpdater( &updateFuseHighSPIEN );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighSPIEN );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_CKOPT", "High", "Oscillator options", false ));
	propPtr->addAcceptedValue( "High", "Clock Option High - See reference manual" );
	propPtr->addAcceptedValue( "Low", "Clock Option Low - See reference manual" );
	propPtr->registerForwardUpdater( &updateFuseHighCKOPT );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighCKOPT );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_EESAVE", "Disabled", "EEPROM memory is preserved through the Chip Erase", false ));
	propPtr->addAcceptedValue( "Enabled", "EEPROM memory isn't erased on a Chip Erase" );
	propPtr->addAcceptedValue( "Disabled", "EEPROM memory will be erased on a Chip Erase" );
	propPtr->registerForwardUpdater( &updateFuseHighEESAVE );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighEESAVE );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_BOOTSZ", "1024w", "Select Boot Size", false));
	propPtr->addAcceptedValue( "128w",  "Boot size is  128 words (0xF80-0xFFF)" );
	propPtr->addAcceptedValue( "256w",  "Boot size is  256 words (0xF00-0xFFF)" );
	propPtr->addAcceptedValue( "512w",  "Boot size is  512 words (0xE00-0xFFF)" );
	propPtr->addAcceptedValue( "1024w", "Boot size is 1024 words (0xC00-0xFFF)" );
	propPtr->registerForwardUpdater( &updateFuseHighBOOTSZ );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighBOOTSZ );
	this->addProperty( propPtr );
	high_memPtr->addMemoryListener( propPtr );

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_BOOTRST", "Vector0000", "Select Reset Vector", false));
	propPtr->addAcceptedValue( "Vector0000",  "Reset vector is set at address 0x0000");
	propPtr->addAcceptedValue( "BootVector",  "Reset Vector is set to the start of the boot loader space");
	propPtr->registerForwardUpdater( &updateFuseHighBOOTRST );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighBOOTRST );
	this->addProperty( propPtr );
	high_memPtr->addMemoryListener( propPtr );

	//Configuration Properties for the Fuse Low byte
	propPtr = shared_ptr<Property>(new Property( this, "FUSELOW_BODLEVEL", "Disabled", "Brown-Out Detector trigger level fuses", false ));
	propPtr->addAcceptedValue( "Disabled", "BOD Disabled" );
	propPtr->addAcceptedValue( "2V7", "Brown-Out at 2.7V" );
	propPtr->addAcceptedValue( "4V0", "Brown-Out at 4.0V" );
	propPtr->registerForwardUpdater( &updateFuseLowBodLevel );
	propPtr->registerReverseUpdater( &reverseUpdateFuseLowBodLevel );
	this->addProperty(propPtr);
	low_memPtr->addMemoryListener(propPtr);

	//TODO Find a better way of dealing with SUT and CKSEL fuses configurations options
	propPtr = shared_ptr<Property>(new Property( this, "FUSELOW_SUT", "10", "Select start-up time", false));
	propPtr->addAcceptedValue( "00", "See System clock and Clock options of the reference manual" );
	propPtr->addAcceptedValue( "01", "See System clock and Clock options of the reference manual" );
	propPtr->addAcceptedValue( "10", "See System clock and Clock options of the reference manual" );
	propPtr->addAcceptedValue( "11", "See System clock and Clock options of the reference manual" );
	propPtr->registerForwardUpdater(&updateFuseLowSUT);
	propPtr->registerReverseUpdater(&reverseUpdateFuseLowSUT);
	this->addProperty(propPtr);
	low_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSELOW_CKSEL", "0001", "Select clock", false));
	propPtr->addAcceptedValue( "0000", "External Clock" );
	propPtr->addAcceptedValue( "0001", "Calibrated Internal RC Oscillator 1.0 MHz" );
	propPtr->addAcceptedValue( "0010", "Calibrated Internal RC Oscillator 2.0 MHz" );
	propPtr->addAcceptedValue( "0011", "Calibrated Internal RC Oscillator 4.0 MHz" );
	propPtr->addAcceptedValue( "0100", "Calibrated Internal RC Oscillator 8.0 MHz" );
	propPtr->addAcceptedValue( "0101", "External RC Oscillator (0.1 - 0.9MHz)");
	propPtr->addAcceptedValue( "0110", "External RC Oscillator (0.9 - 3.0MHz)" );
	propPtr->addAcceptedValue( "0111", "External RC Oscillator (3.0 - 8.0MHz)" );
	propPtr->addAcceptedValue( "1000", "External RC Oscillator (8.9 - 12.0MHz)" );
	propPtr->addAcceptedValue( "1001", "External Low Frequency Crystal (0.4 - 0.9 MHz)  Cfg.1" );
	propPtr->addAcceptedValue( "1010", "External Crystal/Ceramic Resonator (0.4 - 0.9 MHz) with CKOPT=1 Cfg.0" );
	propPtr->addAcceptedValue( "1011", "External Crystal/Ceramic Resonator (0.4 - 0.9 MHz) with CKOPT=1 Cfg.1" );
	propPtr->addAcceptedValue( "1100", "External Crystal/Ceramic Resonator (0.9 - 3.0 MHz) with CKOPT=1 Cfg.0" );
	propPtr->addAcceptedValue( "1101", "External Crystal/Ceramic Resonator (0.9 - 3.0 MHz) with CKOPT=1 Cfg.1" );
	propPtr->addAcceptedValue( "1110", "External Crystal/Ceramic Resonator (3.0 - 8.0 MHz) with CKOPT=1 Cfg.0" );
	propPtr->addAcceptedValue( "1111", "External Crystal/Ceramic Resonator (3.0 - 8.0 MHz) with CKOPT=1 Cfg.1" );
	propPtr->registerForwardUpdater(&updateFuseLowCKSEL);
	propPtr->registerReverseUpdater(&reverseUpdateFuseLowCKSEL);
	this->addProperty(propPtr);
	low_memPtr->addMemoryListener(propPtr);

	/**************************************************************************
	 * Programmer Property: InternalClockSourceActive
	 * Placed last to avoid initialization to the default fuses value "true",
	 * since this may not be coherent with the current real chip configuration,
	 * as would happen with the update generated by the
	 * reverseUpdateInternalClockSource.
	 *
	 * By default for the sake of allowing the communication with a device
	 * which we don't know it's fuses configurations, we assume that the
	 * internal clock source is currently inactive.
	 *************************************************************************/
	progPropPtr = shared_ptr<Property>(new Property( this, "InternalClockSourceActive", "false", "Whether the current chip configuration is using an internal clock", true ));
	progPropPtr->addAcceptedValue( "true", "The chip is currently running from an internal clock source" );
	progPropPtr->addAcceptedValue( "false", "The chip is running from an external clock source (external XTAL/RC/etc)" );
	progPropPtr->registerReverseUpdater( &reverseUpdateInternalClockSource );
	this->addProgrammerProperty( progPropPtr );
	low_memPtr->addMemoryListener( progPropPtr );
}

unsigned long ChipATMega8::getExternalClockFrequency() {
	Property &internalClock = getProgrammerConfigurationProperty("InternalClockSourceActive");
	if ( !Utils::string2bool(internalClock.getValue()) ) {
		return externalClockFrequency;
	} else {
		return 0;
	}
}

void ChipATMega8::setExternalClockFrequency(unsigned long frequency) {
	this->externalClockFrequency = frequency;
}

void ChipATMega8::setInternalClockFrequency(unsigned long newInternalClockFrequency)  {
	internalClockFrequency = newInternalClockFrequency;
}

unsigned long ChipATMega8::getInternalClockFrequency() {
	return internalClockFrequency;
}

ChipATMega8::~ChipATMega8() {
}

InterfaceClass::ChipIF *ChipATMega8::createInstance(const std::string &deviceSpecification) {
	return new ChipATMega8(deviceSpecification);
}

bool ChipATMega8::compareWithFile(const char* filename) {
	return false;
}

bool ChipATMega8::compareWithChip(ChipIF& otherChip) {
	return false;
}

string ChipATMega8::getFactoryNameReference() {
	return "ATMEGA8";
}
