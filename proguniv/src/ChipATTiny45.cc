/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ChipATTiny45.cc
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
#include "../include/ChipATTiny45.hh"
#include "../include/MemoryInterceptorProxy.hh"

ChipATTiny45::ChipATTiny45(const std::string &deviceSpecification) {
	//Add chip memories 
	shared_ptr<Memoria> memPtr =  shared_ptr<Memoria>(new Memoria(4096, 0x0000, 16, "Program"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xffff, 0x0000, 4096);
	
	memPtr = shared_ptr<Memoria>(new Memoria(256, 0x0000, 8, "Data"));
	this->addMemory( memPtr->obtem_nome(), memPtr );
	memPtr->fill_value(0xff, 0x0000, 256);
	
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
			
	//Set ATTiny45 chip signature string
	this->setSignatureString("1E9206");
	
	//Set ATTiny45 chip name
	this->setChipName("ATMEL ATTiny45");
}

ChipATTiny45::~ChipATTiny45() {	
}

InterfaceClass::ChipIF *ChipATTiny45::createInstance(const std::string &deviceSpecification) {
	return new ChipATTiny45(deviceSpecification);
}

bool ChipATTiny45::compareWithFile(const char* filename) {
	return false;
}

bool ChipATTiny45::compareWithChip(ChipIF& otherChip) {
	return false;
}

string ChipATTiny45::getFactoryNameReference() {
	return "ATTINY45";
}
