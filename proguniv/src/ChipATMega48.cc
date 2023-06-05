/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ChipATMega48.cc
 *
 *  Copyright  2007,2008  Lu�s Mendes
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
#include "../include/ChipATMega48.hh"
#include "../include/MemoryInterceptorProxy.hh"

namespace ATMega48 {
	//Handle ATMega48/48A/48PA,ATMega88/88A/88PA,ATMega168/168A/168PA bits 2,1,0 of Fuse High Byte
	void updateFuseHighBodLevel(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse High Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Clear the bits
		old_value &= ~(0x07);

		if ( value.compare("1V8") == 0 ) {
			//bits 2,1,0 = 110
			mem.escreve( old_value | 0x06 );
		}
		else if ( value.compare("2V7") == 0 ) {
			//bits 2,1,0 = 101
			mem.escreve( old_value | 0x05 );
		}
		else if ( value.compare("4V3") == 0 ) {
			//bits 2,1,0 = 100
			mem.escreve( old_value | 0x04 );
		}
		else if ( value.compare("Reserved1") == 0 ) {
			//bits 2,1,0 = 011
			mem.escreve( old_value | 0x03 );
		}
		else if ( value.compare("Reserved2") == 0 ) {
			//bits 2,1,0 = 010
			mem.escreve( old_value | 0x02 );
		}
		else if ( value.compare("Reserved3") == 0 ) {
			//bits 2,1,0 = 001
			mem.escreve( old_value | 0x01 );
		}
		else if ( value.compare("Reserved4") == 0 ) {
			//bits 2,1,0 = 000
			mem.escreve( old_value );
		}
		else {
			//Disabled - bits 2,1,0 = 111
			mem.escreve( old_value | 0x07 );
		}
	}

	std::string reverseUpdateFuseHighBodLevel(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse High Byte");

		mem.le(&value);
		value &= 0x07;

		if ( value == 0x06 ) {
			return "1V8";
		}
		else if ( value == 0x05 ) {
			return "2V7";
		}
		else if ( value == 0x04 ) {
			return "4V3";
		}
		else if ( value == 0x03 ) {
			return "Reserved1";
		}
		else if ( value == 0x02 ) {
			return "Reserved2";
		}
		else if ( value == 0x01 ) {
			return "Reserved3";
		}
		else if ( value == 0x00 ) {
			return "Reserved4";
		}
		else {
			return "Disabled";
		}
	}

	//ATMega48/48A/48PA Extended Fuse Byte
	void updateFuseExtSELFPRGEN(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse Extended Byte");
		unsigned int old_value;
		mem.le(&old_value);
		
		//Allways set default value for unused bits
		old_value |= ~(1 << 0);
		
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
	
	std::string reverseUpdateFuseExtSELFPRGEN(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse Extended Byte");
		
		mem.le(&value);
		
		value &= 0x01;
		
		if ( value == 0x01 ) {
			return "Disabled";
		}
		else
			return "Enabled";
	}
}

ChipATMega48::ChipATMega48(const std::string &deviceSpecification) {
	using namespace ATMega48;
	
	//Add chip memories 
	shared_ptr<Memoria> memPtr =  shared_ptr<Memoria>(new Memoria(2048, 0x0000, 16, "Program"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xffff, 0x0000, 2048);
	
	memPtr = shared_ptr<Memoria>(new Memoria(256, 0x0000, 8, "Data"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xff, 0x0000, 256);
	
	MemoryInterceptorProxy *high_memPtr = (MemoryInterceptorProxy *)this->getMemory("Fuse High Byte");
	MemoryInterceptorProxy *extended_memPtr = (MemoryInterceptorProxy *)this->getMemory("Fuse Extended Byte");
	
	/********************************************
	 * Programmer Properties
	 ********************************************/
	//Add programmer related specific chip properties
	shared_ptr<Property> progPropPtr = shared_ptr<Property>(new Property( this, "ProgramPageSizeWordsBits", "5", "The size in bits of each page for the program memory", true ));
	progPropPtr->addAcceptedValue( "5", "Program memory page size is 32 words" );
	this->addProgrammerProperty( progPropPtr );
	
	progPropPtr = shared_ptr<Property>(new Property( this, "ProgramTotalPages", "64", "The total number of program memory pages in this chip", true ));
	progPropPtr->addAcceptedValue( "64", "The total number of program memory pages is 64" );
	this->addProgrammerProperty( progPropPtr );
	
	progPropPtr = shared_ptr<Property>(new Property( this, "DataPageSizeWordsBits", "2", "The size in bits of each page for the data memory", true ));
	progPropPtr->addAcceptedValue( "2", "Data memory page size is 4 words" );
	this->addProgrammerProperty( progPropPtr );
	
	progPropPtr = shared_ptr<Property>(new Property( this, "DataTotalPages", "64", "The total number of program memory pages in this chip", true ));
	progPropPtr->addAcceptedValue( "64", "The total number of data memory pages is 64" );
	this->addProgrammerProperty( progPropPtr );
	
	/********************************************
	 * Chip Configuration Properties
	 ********************************************/
	//Fuse High Byte properties for ATMega48/48A/48PA,ATMega88/88A/88PA,ATMega168/168A/168PA
	shared_ptr<Property> propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_BODLEVEL", "Disabled", "Brown-Out Detector trigger level fuses", false ));
	propPtr->addAcceptedValue( "Disabled", "BOD Disabled" );
	propPtr->addAcceptedValue( "1V8", "Brown-Out at 1.8V" );
	propPtr->addAcceptedValue( "2V7", "Brown-Out at 2.7V" );
	propPtr->addAcceptedValue( "4V3", "Brown-Out at 4.3V" );
	propPtr->addAcceptedValue( "Reserved1", "Bit's 2,1,0 of the Fuse Extended byte are set to 011" );
	propPtr->addAcceptedValue( "Reserved2", "Bit's 2,1,0 of the Fuse Extended byte are set to 010" );
	propPtr->addAcceptedValue( "Reserved3", "Bit's 2,1,0 of the Fuse Extended byte are set to 001" );
	propPtr->addAcceptedValue( "Reserved4", "Bit's 2,1,0 of the Fuse Extended byte are set to 000" );
	propPtr->registerForwardUpdater( &updateFuseHighBodLevel );
	propPtr->registerReverseUpdater( &reverseUpdateFuseHighBodLevel );
	this->addProperty(propPtr);
	high_memPtr->addMemoryListener(propPtr);

	//Extended Fuse Byte properties
	propPtr = shared_ptr<Property>(new Property( this, "FUSEEXT_SELFPRGEN", "Disabled", "Self-Programming Enable", false ));
	propPtr->addAcceptedValue( "Enabled", "The chip is allowed to program itself" );
	propPtr->addAcceptedValue( "Disabled", "The chip cannot write into program memory" );
	propPtr->registerForwardUpdater( (forwardPtr)&updateFuseExtSELFPRGEN );
	propPtr->registerReverseUpdater( &reverseUpdateFuseExtSELFPRGEN );
	this->addProperty(propPtr);
	extended_memPtr->addMemoryListener(propPtr);
	
	if (std::string("A").compare(deviceSpecification)) {
		//Set ATMega48 chip signature string
		this->setSignatureString("1E9205");

		//Set ATMega48 chip name
		this->setChipName("ATMEL ATMega48A");
	}
	else if (std::string("PA").compare(deviceSpecification)) {
		//Set ATMega48 chip signature string
		this->setSignatureString("1E920A");

		//Set ATMega48 chip name
		this->setChipName("ATMEL ATMega48PA");
	}
	else {
		//Set ATMega48 chip signature string
		this->setSignatureString("1E9205");

		//Set ATMega48 chip name
		this->setChipName("ATMEL ATMega48");
	}
}

ChipATMega48::~ChipATMega48() {	
}

InterfaceClass::ChipIF *ChipATMega48::createInstance(const std::string &deviceSpecification) {
	return new ChipATMega48(deviceSpecification);
}

bool ChipATMega48::compareWithFile(const char* filename) {
	return false;
}

bool ChipATMega48::compareWithChip(ChipIF& otherChip) {
	return false;
}

string ChipATMega48::getFactoryNameReference() {
	return "ATMEGA48";
}
