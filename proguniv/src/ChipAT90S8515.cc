/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ChipAT90S8515.cc
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
#include "../include/Memoria.hh"
#include "../include/ChipIF.hh"
#include "../include/Property.hh"
#include "../include/ChipAT90S8515.hh"
#include "../include/MemoryInterceptorProxy.hh"

namespace AT90S8515 {
	//Handle Lock Bit Byte fuses
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
	void updateFuseHighSPIEN(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);
		
		//Set unused bits
		old_value |= ~(0x21);
		
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

	void updateFuseHighFSTRT(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Set unused bits
		old_value |= ~(0x21);
		
		//Clear the bits
		old_value &= ~(1 << 0);
		
		if ( value.compare("Disabled") == 0 ) {
			mem.escreve( old_value | (1 << 0) );
		}
		else {
			//Enabled
			mem.escreve( old_value );
		}
	}
	
	std::string reverseUpdateFuseHighFSTRT(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		
		mem.le(&value);
		
		value &= 0x01;
		
		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
	}
	
}

ChipAT90S8515::ChipAT90S8515(const std::string &deviceSpecification) {
	using namespace AT90S8515;
	
	//Add chip memories 
	shared_ptr<Memoria> memPtr =  shared_ptr<Memoria>(new Memoria(4096, 0x0000, 16, "Program"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xffff, 0x0000, 4096);
	
	memPtr = shared_ptr<Memoria>(new Memoria(512, 0x0000, 8, "Data"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xff, 0x0000, 512);
	
	shared_ptr<MemoryInterceptorProxy> lock_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Lock Bit Byte"));
	this->addMemory( lock_memPtr->obtem_nome(), static_pointer_cast<Memoria,MemoryInterceptorProxy>(lock_memPtr) );
	
	shared_ptr<MemoryInterceptorProxy> high_memPtr = shared_ptr<MemoryInterceptorProxy>(new MemoryInterceptorProxy(8, "Fuse High Byte"));
	this->addMemory( high_memPtr->obtem_nome(), static_pointer_cast<Memoria,MemoryInterceptorProxy>(high_memPtr) );
		
	/********************************************
	 * Programmer Properties
	 ********************************************/
	//Add programmer related specific chip properties
	shared_ptr<Property> progPropPtr = shared_ptr<Property>(new Property( this, "MemoryType", "NonPaged", "Tells wether this chip contains paged memories or not", true ));
	progPropPtr->addAcceptedValue( "NonPaged", "This chip memory is of the Non-Paged Flash type" );
	this->addProgrammerProperty( progPropPtr );
		
	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_flash", "9000", "The amount of time required to program the flash memory at minimun Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "9000", "9.0 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_eeprom", "9000", "The amount of time required to program the eeprom memory at minimun Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "9000", "9.0 ms" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_erase", "22000", "The amount of time required to erase memory at minimun Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "22000", "22.0 ms" );
	this->addProgrammerProperty( progPropPtr );
	
	progPropPtr = shared_ptr<Property>(new Property( this, "minVddTwd_fuse", "9000", "The amount of time required to program fuse memory at minimun Vdd voltage", true ));
	progPropPtr->addAcceptedValue( "9000", "9.0 ms" );
	this->addProgrammerProperty( progPropPtr );
	
	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockDelay", "1", "Minimum amount of time that each clock half-cycle must take", true ));
	progPropPtr->addAcceptedValue( "1", "1 us" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesSCK0", "3", "Clock cycles to generate per bit when SCK is 0", true ));
	progPropPtr->addAcceptedValue( "3", "3 cycles" );
	this->addProgrammerProperty( progPropPtr );

	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesWriteSCK1", "3", "Clock cycles to generate per bit when SCK is 1 on a Write", true ));
	progPropPtr->addAcceptedValue( "3", "3 cycles" );
	this->addProgrammerProperty( progPropPtr );
	
	progPropPtr = shared_ptr<Property>(new Property( this, "serialClockCyclesReadSCK1", "3", "Clock cycles to generate per bit when SCK is 1 on a Read", true ));
	progPropPtr->addAcceptedValue( "3", "3 cycles" );
	this->addProgrammerProperty( progPropPtr );
		
	progPropPtr = shared_ptr<Property>(new Property( this, "HasInternalClockSource", "false", "Wether this chip can run from an internal clock", true ));
	progPropPtr->addAcceptedValue( "false", "This chip can run from an internal clock source" );
	//progPropPtr->addAcceptedValue( "false", "This chip can only run from an external clock source" );
	this->addProgrammerProperty( progPropPtr );
	
	progPropPtr = shared_ptr<Property>(new Property( this, "InternalClockSourceActive", "false", "Wether the current chip configuration is using an internal clock", true ));
	progPropPtr->addAcceptedValue( "false", "The chip is currently running from an internal clock source" );
	//progPropPtr->addAcceptedValue( "false", "The chip is running from an external clock source (external XTAL/RC/etc)" );
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
	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_SPIEN", "Enabled", "Enable Serial Program and Data Downloading", false ));
	propPtr->addAcceptedValue( "Enabled", "Serial Program and Data Downloading enabled" );
	propPtr->addAcceptedValue( "Disabled", "Serial Program and Data Downloading disabled" );
	propPtr->registerForwardUpdater( &updateFuseHighSPIEN );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighSPIEN );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_FSTRT", "Disabled", "Enable short start-up time", false ));
	propPtr->addAcceptedValue( "Enabled", "Fast Start is selected" );
	propPtr->addAcceptedValue( "Disabled", "Normal Start is selected" );
	propPtr->registerForwardUpdater( &updateFuseHighFSTRT );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighFSTRT );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);
		
	//Set ATTiny461 chip signature string
	this->setSignatureString("1E9301");
	
	//Set ATTiny461 chip name
	this->setChipName("ATMEL AT90S8515");
}


ChipAT90S8515::~ChipAT90S8515() {	
}

InterfaceClass::ChipIF *ChipAT90S8515::createInstance(const std::string &deviceSpecification) {
	return new ChipAT90S8515(deviceSpecification);
}

bool ChipAT90S8515::compareWithFile(const char* filename) {
	return false;
}

bool ChipAT90S8515::compareWithChip(ChipIF& otherChip) {
	return false;
}

string ChipAT90S8515::getFactoryNameReference() {
	return "AT90S8515";
}
