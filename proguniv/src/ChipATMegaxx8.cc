/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            ChipATMegaxx8.cc
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
#include "../include/ChipATMegaxx8.hh"
#include "../include/MemoryInterceptorProxy.hh"

namespace ATMegaxx8 {
	//Handle Lock Bit Byte fuses
	//TODO Add constraint checks  -> DWEN cannot be enabled when using lock bits
	//	(for example in section 22.2 page 169 of the reference manual)
	void updateLockBit(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Lock Bit Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Set unused bits
		old_value |= ~(0x03);

		//Clear the bits
		old_value &= ~(0x03);

		if ( value.compare("Mode1") == 0 ) {
			//Set bits 1,0 = "11"
			mem.escreve(old_value | 0x03);
		}
		else if ( value.compare("Mode2") == 0 ) {
			//Set bits 1,0 = "10"
			mem.escreve(old_value | 0x02);
		}
		else if ( value.compare("Mode3") == 0 ) {
			//Set bits 1,0 = "00"
			mem.escreve(old_value | 0x00);
		}
		else {
			//Undocumented, bits 1,0 = "01"
			mem.escreve(old_value | 0x01);
		}
	}

	std::string reverseUpdateLockBit(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Lock Bit Byte");

		mem.le(&value);

		//Extract useful bits
		value &= 0x03;

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

	void updateFuseHighDWEN(const std::string &value, InterfaceClass::ChipIF& chip) {
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

	std::string reverseUpdateFuseHighDWEN(InterfaceClass::ChipIF& chip) {
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

	void updateFuseHighWDTON(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(1 << 4);

		if ( value.compare("Disabled") == 0 ) {
			mem.escreve( old_value | (1 << 4) );
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

		value >>= 4;
		value &= 0x01;

		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
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

	//Bits 2,1,0 of Fuse High Byte vary between ATMega328/328P and ATMega48/48A/48PA/88/88A/88PA/168/168A/168PA

	//Handle Fuse Low Byte
	void updateFuseLowCKDIV8(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse Low Byte");
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

	std::string reverseUpdateFuseLowCKDIV8(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse Low Byte");

		mem.le(&value);

		value >>= 7;
		value &= 0x01;

		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
	}

	void updateFuseLowCKOUT(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse Low Byte");
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

	std::string reverseUpdateFuseLowCKOUT(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse Low Byte");

		mem.le(&value);

		value >>= 6;
		value &= 0x01;

		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
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
    	//This chip has three internal clock sources one of aprox. 128KHz
    	//and another of aprox. 8MHz, the first one is too slow to be used for
    	//programming purposes, so only the 8MHz clock source is considered

    	//0011 - 128KHz internal clock source
    	//0010 - 8MHz calibrated internal clock source
    	std::string result = reverseUpdateFuseLowCKSEL(chip);
    	if ( result.compare("0010") == 0 ) {
    		chip.setInternalClockFrequency(8000000);
    		return "true";
    	} else if (result.compare("0011") == 0) {
    		chip.setInternalClockFrequency(128000);
    		return "true";
    	} else {
    		return "false";
    	}
    }
}

ChipATMegaxx8::ChipATMegaxx8() {
	using namespace ATMegaxx8;

	//Add chip memories
	shared_ptr<MemoryInterceptorProxy> lock_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Lock Bit Byte"));
	this->addMemory( lock_memPtr->obtem_nome(), static_pointer_cast<Memoria, MemoryInterceptorProxy>(lock_memPtr) );

	shared_ptr<MemoryInterceptorProxy> extended_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Fuse Extended Byte"));
	this->addMemory( extended_memPtr->obtem_nome(), static_pointer_cast<Memoria, MemoryInterceptorProxy>(extended_memPtr) );

	shared_ptr<MemoryInterceptorProxy> high_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Fuse High Byte"));
	this->addMemory( high_memPtr->obtem_nome(), static_pointer_cast<Memoria, MemoryInterceptorProxy>(high_memPtr) );

	shared_ptr<MemoryInterceptorProxy> low_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Fuse Low Byte"));
	this->addMemory( low_memPtr->obtem_nome(), static_pointer_cast<Memoria, MemoryInterceptorProxy>(low_memPtr) );

	shared_ptr<Memoria> memPtr = shared_ptr<Memoria>(new Memoria(8, "Calibration Byte"));
	this->addMemory( memPtr->obtem_nome(), memPtr );

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

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_eeprom", "4100", "The amount of time required to program the eeprom memory at minimum Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "4100", "4.1 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_erase", "9500", "The amount of time required to erase memory at minimum Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "9500", "9.5 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_fuse", "4500", "The amount of time required to program fuse memory at minimum Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "4500", "4.5 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "HasInternalClockSource", "true", "Wether this chip can run from an internal clock", true ));
	progPropPtr->addAcceptedValue( "true", "This chip can run from an internal clock source" );
	//progPropPtr->addAcceptedValue( "false", "This chip can only run from an external clock source" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockDelay", "1", "Minimum amount of time that each clock half-cycle must take", true ));
	progPropPtr->addAcceptedValue( "1", "1 us" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesSCK0", "8", "Clock cycles to generate per bit when SCK is 0", true ));
	progPropPtr->addAcceptedValue( "8", "8 cycles" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesWriteSCK1", "16", "Clock cycles to generate per bit when SCK is 1 on a Write", true ));
	progPropPtr->addAcceptedValue( "16", "16 cycles" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesReadSCK1", "8", "Clock cycles to generate per bit when SCK is 1 on a Read", true ));
	progPropPtr->addAcceptedValue( "8", "8 cycles" );
	this->addProgrammerProperty( progPropPtr );


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

	//Fuse High Byte
	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_RSTDISBL", "Disabled", "External Reset disable", false ));
	propPtr->addAcceptedValue( "Enabled", "The external reset pin is disconnected" );
	propPtr->addAcceptedValue( "Disabled", "The external reset pin is in use" );
	propPtr->registerForwardUpdater( &updateFuseHighRSTDISBL );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighRSTDISBL );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_DWEN", "Disabled", "DebugWIRE Enable", false ));
	propPtr->addAcceptedValue( "Enabled", "DebugWIRE is enbled" );
	propPtr->addAcceptedValue( "Disabled", "DebugWIRE functionality is disabled" );
	propPtr->registerForwardUpdater( &updateFuseHighDWEN );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighDWEN );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_SPIEN", "Enabled", "Enable Serial Program and Data Downloading", false ));
	propPtr->addAcceptedValue( "Enabled", "Serial Program and Data Downloading enabled" );
	propPtr->addAcceptedValue( "Disabled", "Serial Program and Data Downloading disabled" );
	propPtr->registerForwardUpdater( &updateFuseHighSPIEN );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighSPIEN );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_WDTON", "Disabled", "Watchdog Timer always on", false ));
	propPtr->addAcceptedValue( "Enabled", "Watchdog Timer is always on" );
	propPtr->addAcceptedValue( "Disabled", "Watchdog Timer is controlled by software" );
	propPtr->registerForwardUpdater( &updateFuseHighWDTON );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighWDTON );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_EESAVE", "Disabled", "EEPROM memory is preserved through the Chip Erase", false ));
	propPtr->addAcceptedValue( "Enabled", "EEPROM memory isn't erased on a Chip Erase" );
	propPtr->addAcceptedValue( "Disabled", "EEPROM memory will be erased on a Chip Erase" );
	propPtr->registerForwardUpdater( &updateFuseHighEESAVE );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighEESAVE );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	//Configuration Properties for the Fuse Low byte
	propPtr = shared_ptr<Property>(new Property( this, "FUSELOW_CKDIV8", "Enabled", "Divide clock by 8", false));
	propPtr->addAcceptedValue( "Disabled", "Clock rate is 1:1" );
	propPtr->addAcceptedValue( "Enabled",  "Clock rate is divided by 8" );
	propPtr->registerForwardUpdater(&updateFuseLowCKDIV8);
	propPtr->registerReverseUpdater(&reverseUpdateFuseLowCKDIV8);
	this->addProperty(propPtr);
	low_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSELOW_CKOUT", "Disabled", "Clock output", false));
	propPtr->addAcceptedValue( "Disabled", "Clock output pin is for general I/O" );
	propPtr->addAcceptedValue( "Enabled",  "Clock output pin is for clock signal" );
	propPtr->registerForwardUpdater(&updateFuseLowCKOUT);
	propPtr->registerReverseUpdater(&reverseUpdateFuseLowCKOUT);
	this->addProperty(propPtr);
	low_memPtr->addMemoryListener(propPtr);

	//TODO Find a better way of dealing with SUT and CKSEL fuses configurations options
	propPtr = shared_ptr<Property>(new Property( this, "FUSELOW_SUT", "10", "Select start-up time", false));
	propPtr->addAcceptedValue( "00", "See section 7.3 of the reference manual" );
	propPtr->addAcceptedValue( "01", "See section 7.3 of the reference manual" );
	propPtr->addAcceptedValue( "10", "See section 7.3 of the reference manual" );
	propPtr->addAcceptedValue( "11", "See section 7.3 of the reference manual" );
	propPtr->registerForwardUpdater(&updateFuseLowSUT);
	propPtr->registerReverseUpdater(&reverseUpdateFuseLowSUT);
	this->addProperty(propPtr);
	low_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSELOW_CKSEL", "0010", "Select clock", false));
	propPtr->addAcceptedValue( "0000", "External Clock" );
	propPtr->addAcceptedValue( "0001", "Reserved" );
	propPtr->addAcceptedValue( "0010", "Calibrated Internal RC Oscillator 8.0 MHz" );
	propPtr->addAcceptedValue( "0011", "Internal 128 kHz RC Oscillator" );
	propPtr->addAcceptedValue( "0100", "Low Frequency Crystal Oscillator (32,768KHz)  Cfg.0" );
	propPtr->addAcceptedValue( "0101", "Low Frequency Crystal Oscillator (32,768KHz)  Cfg.1");
	propPtr->addAcceptedValue( "0110", "Full Swing Crystal Oscillator (0.4 - 20MHz)   Cfg.0" );
	propPtr->addAcceptedValue( "0111", "Full Swing Crystal Oscillator (0.4 - 20MHz)   Cfg.1" );
	propPtr->addAcceptedValue( "1000", "Low Power Crystal Oscillator (0.4 - 0.9 MHz)  Cfg.0" );
	propPtr->addAcceptedValue( "1001", "Low Power Crystal Oscillator (0.4 - 0.9 MHz)  Cfg.1" );
	propPtr->addAcceptedValue( "1010", "Low Power Crystal Oscillator (0.9 - 3.0 MHz)  Cfg.0" );
	propPtr->addAcceptedValue( "1011", "Low Power Crystal Oscillator (0.9 - 3.0 MHz)  Cfg.1" );
	propPtr->addAcceptedValue( "1100", "Low Power Crystal Oscillator (3.0 - 8.0 MHz)  Cfg.0" );
	propPtr->addAcceptedValue( "1101", "Low Power Crystal Oscillator (3.0 - 8.0 MHz)  Cfg.1" );
	propPtr->addAcceptedValue( "1110", "Low Power Crystal Oscillator (8.0 - 16.0 MHz) Cfg.0" );
	propPtr->addAcceptedValue( "1111", "Low Power Crystal Oscillator (8.0 - 16.0 MHz) Cfg.1" );
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

	externalClockFrequency = 0;
	internalClockFrequency = 0;
}

unsigned long ChipATMegaxx8::getExternalClockFrequency() {
	Property &internalClock = getProgrammerConfigurationProperty("InternalClockSourceActive");
	if ( !Utils::string2bool(internalClock.getValue()) ) {
		return externalClockFrequency;
	} else {
		return 0;
	}
}

void ChipATMegaxx8::setExternalClockFrequency(unsigned long frequency) {
	this->externalClockFrequency = frequency;
}

void ChipATMegaxx8::setInternalClockFrequency(unsigned long newInternalClockFrequency)  {
	internalClockFrequency = newInternalClockFrequency;
}

unsigned long ChipATMegaxx8::getInternalClockFrequency() {
	return internalClockFrequency;
}

ChipATMegaxx8::~ChipATMegaxx8() {
}

bool ChipATMegaxx8::compareWithFile(const char* filename) {
	return false;
}

bool ChipATMegaxx8::compareWithChip(ChipIF& otherChip) {
	return false;
}
