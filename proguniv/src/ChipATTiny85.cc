/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ChipATTiny85.cc
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
#include "../include/ChipATTiny85.hh"
#include "../include/MemoryInterceptorProxy.hh"

ChipATTiny85::ChipATTiny85(const std::string &deviceSpecification) {
	//Add chip memories 
	shared_ptr<Memoria> memPtr =  shared_ptr<Memoria>(new Memoria(8192, 0x0000, 16, "Program"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xffff, 0x0000, 8192);
	
	memPtr = shared_ptr<Memoria>(new Memoria(512, 0x0000, 8, "Data"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xff, 0x0000, 512);
	
	/********************************************
	 * Programmer Properties
	 ********************************************/
	//Add programmer related specific chip properties
	shared_ptr<Property> progPropPtr = shared_ptr<Property>(new Property( this, "ProgramPageSizeWordsBits", "5", "The size in bits of each page for the program memory", true ));
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
			
	//Set ATTiny85 chip signature string
	this->setSignatureString("1E930B");
	
	//Set ATTiny85 chip name
	this->setChipName("ATMEL ATTiny85");
}

ChipATTiny85::~ChipATTiny85() {	
}

InterfaceClass::ChipIF *ChipATTiny85::createInstance(const std::string &deviceSpecification) {
	return new ChipATTiny85(deviceSpecification);
}

bool ChipATTiny85::compareWithFile(const char* filename) {
	return false;
}

bool ChipATTiny85::compareWithChip(ChipIF& otherChip) {
	return false;
}

string ChipATTiny85::getFactoryNameReference() {
	return "ATTINY85";
}

