/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            ChipATMega168.cc
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
#include "../include/ChipATMega168.hh"
#include "../include/MemoryInterceptorProxy.hh"

namespace ATMega168 {
	//Handle ATMega88/88A/88PA,ATMega168/168A/168PA Fuses for Lock Bit Byte fuses
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
			mem.escreve( old_value | (0x03 << 2) );
		}
		else if ( !value.compare("Mode2") ) {
			//Set bits 3,2 = "10"
			mem.escreve( old_value | (0x02 << 2) );
		}
		else if ( !value.compare("Mode3") ) {
			//Set bits 3,2 = "00"
			mem.escreve( old_value | (0x00 << 2) );
		}
		else if ( !value.compare("Mode4") ) {
			//Set bits 3,2 = "01"
			mem.escreve( old_value | (0x01 << 2) );
		}
	}

	std::string reverseUpdateBootLockBit0(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Lock Bit Byte");

		mem.le(&value);
		//Get bits 2 and 3
		value >>= 2;
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
		old_value &= (~ (0x03 << 4));

		if ( value.compare("Mode1") == 0 ) {
			//Set bits 5,4 = "11"
			mem.escreve( old_value | (0x03 << 4) );
		}
		else if ( value.compare("Mode2") == 0 ) {
			//Set bits 5,4 = "10"
			mem.escreve( old_value | (0x02 << 4) );
		}
		else if ( value.compare("Mode3") == 0 ) {
			//Set bits 5,4 = "00"
			mem.escreve( old_value | (0x00 << 4) );
		}
		else if ( value.compare("Mode4") == 0 ) {
			//Set bits 5,4 = "01"
			mem.escreve( old_value | (0x01 << 4) );
		}
	}

	std::string reverseUpdateBootLockBit1(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Lock Bit Byte");

		mem.le(&value);
		//Get bits 4 and 5
		value >>= 4;
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
			//Value == 1
			return "Mode4";
		}
	}

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


	//Handle ATMega88/168 Fuse Extended Byte
	void updateFuseExtBOOTSZ(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse Extended Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Set unused bits
		old_value |= (0x1F << 3);

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

	std::string reverseUpdateFuseExtBOOTSZ(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse Extended Byte");

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

	void updateFuseExtBOOTRST(const std::string &value, InterfaceClass::ChipIF& chip) {
		Memoria& mem = *chip.getMemory("Fuse Extended Byte");
		unsigned int old_value;
		mem.le(&old_value);

		//Set unused bits
		old_value |= (0x1F << 3);

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

	std::string reverseUpdateFuseExtBOOTRST(InterfaceClass::ChipIF& chip) {
		unsigned int value;
		Memoria& mem = *chip.getMemory("Fuse Extended Byte");

		mem.le(&value);

		value &= 0x01;

		if ( value == 0x01 ) {
			return "Vector0000";
		}
		else {
			return "BootVector";
		}
	}
}

ChipATMega168::ChipATMega168(const std::string &deviceSpecification) : ChipATMegaxx8() {
	using namespace ATMega168;

	//Add chip memories
	shared_ptr<Memoria> memPtr =  shared_ptr<Memoria>(new Memoria(8192, 0x0000, 16, "Program"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xffff, 0x0000, 8192);

	memPtr = shared_ptr<Memoria>(new Memoria(512, 0x0000, 8, "Data"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xff, 0x0000, 512);

	MemoryInterceptorProxy *lock_memPtr = (MemoryInterceptorProxy *)this->getMemory("Lock Bit Byte");
	MemoryInterceptorProxy *high_memPtr = (MemoryInterceptorProxy *)this->getMemory("Fuse High Byte");
	MemoryInterceptorProxy *extended_memPtr = (MemoryInterceptorProxy *)this->getMemory("Fuse Extended Byte");

	/********************************************
	 * Programmer Properties
	 ********************************************/
	//Add programmer related specific chip properties
	shared_ptr<Property> progPropPtr = shared_ptr<Property>(new Property( this, "ProgramPageSizeWordsBits", "6", "The size in bits of each page for the program memory", true ));
	progPropPtr->addAcceptedValue( "6", "Program memory page size is 64 words" );
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


	/********************************************
	 * Chip Configuration Properties
	 ********************************************/
	//Configuration Properties for the LOCK byte
	shared_ptr<Property> propPtr = shared_ptr<Property>(new Property( this, "LOCKBYTE_BOOTLOCKBIT0", "Mode1", "Memory Boot Lock Options #1", false ));
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

	//Fuse High Byte properties for ATMega48/48A/48PA,ATMega88/88A/88PA,ATMega168/168A/168PA
	propPtr = shared_ptr<Property>(new Property( this, "FUSEHIGH_BODLEVEL", "Disabled", "Brown-Out Detector trigger level fuses", false ));
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
	propPtr = shared_ptr<Property>(new Property( this, "FUSEEXT_BOOTSZ", "1024w", "Select Boot Size", false));
	propPtr->addAcceptedValue( "128w",  "Boot size is  128 words (0x1F80-0x1FFF)" );
	propPtr->addAcceptedValue( "256w",  "Boot size is  256 words (0x1F00-0x1FFF)" );
	propPtr->addAcceptedValue( "512w",  "Boot size is  512 words (0x1E00-0x1FFF)" );
	propPtr->addAcceptedValue( "1024w", "Boot size is 1024 words (0x1C00-0x1FFF)" );
	propPtr->registerForwardUpdater( &updateFuseExtBOOTSZ );
	propPtr->registerReverseUpdater( &reverseUpdateFuseExtBOOTSZ );
	this->addProperty( propPtr );
	extended_memPtr->addMemoryListener( propPtr );

	propPtr = shared_ptr<Property>(new Property( this, "FUSEEXT_BOOTRST", "Vector0000", "Select Reset Vector", false));
	propPtr->addAcceptedValue( "Vector0000",  "Reset vector is set at address 0x0000");
	propPtr->addAcceptedValue( "BootVector",  "Reset Vector is set to the start of the boot loader space");
	propPtr->registerForwardUpdater( &updateFuseExtBOOTRST );
	propPtr->registerReverseUpdater( &reverseUpdateFuseExtBOOTRST );
	this->addProperty( propPtr );
	extended_memPtr->addMemoryListener( propPtr );


	if (std::string("A").compare(deviceSpecification) == 0) {
		//Set ATTiny461 chip signature string
		this->setSignatureString("1E9406");

		//Set ATTiny461 chip name
		this->setChipName("ATMEL ATMega168A");
	}
	else if (std::string("PA").compare(deviceSpecification) == 0) {
		//Set ATTiny461 chip signature string
		this->setSignatureString("1E940B");

		//Set ATTiny461 chip name
		this->setChipName("ATMEL ATMega168PA");
	}
	else {
		//Set ATTiny461 chip signature string
		this->setSignatureString("1E9406");

		//Set ATTiny461 chip name
		this->setChipName("ATMEL ATMega168");
	}
}

ChipATMega168::~ChipATMega168() {
}

InterfaceClass::ChipIF *ChipATMega168::createInstance(const std::string &deviceSpecification) {
	return new ChipATMega168(deviceSpecification);
}

bool ChipATMega168::compareWithFile(const char* filename) {
	return false;
}

bool ChipATMega168::compareWithChip(ChipIF& otherChip) {
	return false;
}

string ChipATMega168::getFactoryNameReference() {
	return "ATMEGA168";
}
