/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            Chip.cc
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

#include <string.h>
#include <iostream>
#include <utility>
#include <memory>
#include <string>
#include <list>
#include "../include/ChipIF.hh"
#include "../include/Chip.hh"
#include "../include/ChipATTiny25.hh"
#include "../include/ChipATTiny45.hh"
#include "../include/ChipATTiny85.hh"
#include "../include/ChipATTiny261.hh"
#include "../include/ChipATTiny461.hh"
#include "../include/ChipATTiny861.hh"
#include "../include/ChipATMega8.hh"
#include "../include/ChipATMega48.hh"
#include "../include/ChipATMega48A.hh"
#include "../include/ChipATMega48PA.hh"
#include "../include/ChipATMega88.hh"
#include "../include/ChipATMega88A.hh"
#include "../include/ChipATMega88PA.hh"
#include "../include/ChipATMega168.hh"
#include "../include/ChipATMega168A.hh"
#include "../include/ChipATMega168PA.hh"
#include "../include/ChipATMega328.hh"
#include "../include/ChipATMega328P.hh"
#include "../include/ChipATMega32U4.hh"
#include "../include/ChipATMega644.hh"
#include "../include/ChipATMega2560.hh"
#include "../include/ChipAT90Usb1286.hh"
#include "../include/ChipAT90S2313.hh"
#include "../include/ChipAT90S8515.hh"
#include "../include/InvalidFileException.hh"

//These types are also declared in ChipIF.hh
typedef std::list<std::string> programmerPropertiesIndexList_t;
typedef std::list<std::string> propertiesIndexList_t;
typedef std::list<std::string> memoriesIndexList_t;

using namespace std;

namespace InterfaceClass {

   	/**
	 * Base chip factory method
	 * Should have a list of all plugin classes factories and delegate
	 * to them for instantiation when necessary
	 */
	std::shared_ptr<ChipIF> ChipIF::createInstance(string manufacturer, string chip_ref) {
		std::call_once(staticChip::factoryMethodsMapsOnceFlag, &Chip::initFactoryMaps);

		shared_ptr<std::map<std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>>> map =
				staticChip::factoryMethodsMaps[manufacturer];
		if (map != 0) {
			tr1::function<InterfaceClass::ChipIF*(string)> fun = (*map)[chip_ref];
			if (fun != 0) {
				return shared_ptr<InterfaceClass::ChipIF>(fun(""));
			}
		}

		return 0;
	}
}

void Chip::initFactoryMaps() {
	using namespace staticChip;

	//Create ATMEL entry
	std::map<std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>> *atmelFactoriesMap =
			new std::map<std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>>();
	auto ptr = shared_ptr<std::map<std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>>>(atmelFactoriesMap);
	std::pair<string, shared_ptr<std::map<std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>>>> insertPair("ATMEL", ptr);
	factoryMethodsMaps.insert(insertPair);

	//Insert ATMEL chips
	std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>> chipPair(ChipATTiny25::getFactoryNameReference(), &ChipATTiny25::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATTiny45::getFactoryNameReference(), &ChipATTiny45::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATTiny85::getFactoryNameReference(), &ChipATTiny85::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATTiny261::getFactoryNameReference(), &ChipATTiny261::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATTiny461::getFactoryNameReference(), &ChipATTiny461::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATTiny861::getFactoryNameReference(), &ChipATTiny861::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega8::getFactoryNameReference(), &ChipATMega8::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega48::getFactoryNameReference(), &ChipATMega48::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega48A::getFactoryNameReference(), &ChipATMega48A::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega48PA::getFactoryNameReference(), &ChipATMega48PA::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega88::getFactoryNameReference(), &ChipATMega88::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega88A::getFactoryNameReference(), &ChipATMega88A::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega88PA::getFactoryNameReference(), &ChipATMega88PA::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega168::getFactoryNameReference(), &ChipATMega168::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega168A::getFactoryNameReference(), &ChipATMega168A::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega168PA::getFactoryNameReference(), &ChipATMega168PA::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega328::getFactoryNameReference(), &ChipATMega328::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega328P::getFactoryNameReference(), &ChipATMega328P::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega32U4::getFactoryNameReference(), &ChipATMega32U4::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega644::getFactoryNameReference(), &ChipATMega644::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipATMega2560::getFactoryNameReference(), &ChipATMega2560::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipAT90Usb1286::getFactoryNameReference(), &ChipAT90Usb1286::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipAT90S2313::getFactoryNameReference(), &ChipAT90S2313::createInstance);
	atmelFactoriesMap->insert(chipPair);
	chipPair=std::pair<string, tr1::function<InterfaceClass::ChipIF*(std::string)>>(ChipAT90S8515::getFactoryNameReference(), &ChipAT90S8515::createInstance);
	atmelFactoriesMap->insert(chipPair);
}

string Chip::listKnownChips() {
	using namespace staticChip;
	string result = "";

	std::call_once(staticChip::factoryMethodsMapsOnceFlag, &Chip::initFactoryMaps);

	auto manufacturerIterator = factoryMethodsMaps.begin();
	for (;!factoryMethodsMaps.empty() && manufacturerIterator != factoryMethodsMaps.end(); manufacturerIterator++) {
		std::pair<const string, shared_ptr<std::map<std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>>>>& p =
				*manufacturerIterator;
		const string manufacturer = p.first;
		shared_ptr<std::map<std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>>> chipMap = p.second;

		result += "Manufacturer: " + manufacturer + "\n";
		auto chipIterator = chipMap->begin();
		for (; !chipMap->empty() && chipIterator != chipMap->end(); chipIterator++) {
			pair<const std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>>& chipPair = *chipIterator;
			const string chipFactoryNameReference = chipPair.first;
			result += "     Chip factory name: " + chipFactoryNameReference + "\n";
		}
	}

	return result;
}

Chip::~Chip() {
	//Iterate over all Memories and over all Properties in order to delete them
/*	propertiesMap_t::iterator i = propertiesMap.begin();
	propertiesMap_t::iterator old;
	Property *oldProp;
	while ( !propertiesMap.empty() && i != propertiesMap.end() ) {
		oldProp = &((*i).second);
		old = i;
		++i;
		propertiesMap.erase(old);
		if (oldProp != 0) {
			delete oldProp;
		}
	}

	memoriesMap_t::iterator iMem = memoriesMap.begin();
	const Memoria *oldMem;
	while ( !memoriesMap.empty() && iMem != memoriesMap.end() ) {
		oldMem = ((*iMem).second);
		if (oldMem != 0)
			delete oldMem;
		memoriesMap.erase(iMem);
		++iMem;
	}*/

	if (signatureString != 0)
		delete signatureString;
}

Chip::Chip() {
	signatureString = 0;
	chipName = new std::string("Unnamed Chip");
}

const std::string &Chip::getChipName() {
	return *chipName;
}

std::unique_ptr<memoriesIndexList_t> Chip::getMemoriesList() {
	std::unique_ptr<memoriesIndexList_t> memoriesList( new memoriesIndexList_t(memoriesMap.size()) );

	//Fill in the list to return
	memoriesMap_t::iterator i = memoriesMap.begin();
	while ( i != memoriesMap.end() ) {
		memoriesList->insert( memoriesList->begin(), (*i).first );
	}

	return memoriesList;
}

Memoria *Chip::getMemory(const std::string &memoryName) const {
	memoriesMap_t::const_iterator i = memoriesMap.find(memoryName);

	return ( i != memoriesMap.end() ) ? ((*i).second).get() : 0;
}

std::unique_ptr<programmerPropertiesIndexList_t> Chip::getProgrammerPropertyList() {
	std::unique_ptr<programmerPropertiesIndexList_t> programmerPropertiesList( new programmerPropertiesIndexList_t(programmerPropertiesMap.size()) );

	//Fill in the list to return
	programmerPropertiesMap_t::iterator i = programmerPropertiesMap.begin();
	while ( i != programmerPropertiesMap.end() ) {
		programmerPropertiesList->insert( programmerPropertiesList->begin(), (*i).first );
	}

	return programmerPropertiesList;
}

bool Chip::hasProgrammerConfigurationProperty(const std::string &programmerPropertyName) {
	programmerPropertiesMap_t::iterator i = programmerPropertiesMap.find(programmerPropertyName);
	if ( i != programmerPropertiesMap.end() ) {
		return true;
	}
	return false;
}

Property &Chip::getProgrammerConfigurationProperty(const std::string &programmerPropertyName) {
	programmerPropertiesMap_t::iterator i = programmerPropertiesMap.find(programmerPropertyName);
	shared_ptr<Property> result = 0;

	if ( i != programmerPropertiesMap.end() ) {
		result = (*i).second;
	}
	else {
		std::string error = "Property name (" + programmerPropertyName + ") not found";
		throw InvalidPropertyNameException( error );
	}

	return *(result.get());
}

std::shared_ptr<propertiesIndexList_t> Chip::getPropertyList() {
	std::shared_ptr<propertiesIndexList_t> propertiesList( new propertiesIndexList_t(propertiesMap.size()) );

	//Fill in the list to return
	propertiesMap_t::iterator i = propertiesMap.begin();
	for ( ; !propertiesMap.empty() && i != propertiesMap.end(); i++ ) {
		propertiesList->push_back( (*i).first );
	}

	return propertiesList;
}

void Chip::setConfigurationProperty(const std::string &propertyName, const std::string &propertyValue) {
	propertiesMap_t::iterator i = propertiesMap.find(propertyName);
	shared_ptr<Property> oldProp = 0;

	if ( i != propertiesMap.end() ) {
		oldProp = (*i).second;
		oldProp->setValue(propertyValue);
	}
	else {
		std::string error = "Property name (" + propertyName + ") not found";
		throw InvalidPropertyNameException( error );
	}
}

const Property &Chip::getConfigurationProperty(const std::string &propertyName) {
	propertiesMap_t::const_iterator i = propertiesMap.find(propertyName);
	shared_ptr<Property> result;

	if ( i != propertiesMap.end() ) {
		result = (*i).second;
	}
	else {
		std::string error = "Property name (" + propertyName + ") not found";
		throw InvalidPropertyNameException( error );
	}

	return *(result.get());
}

bool Chip::checkSignature(const std::string &otherSignatureString) {
	if (signatureString == 0)
		return false;

	if ( (*signatureString).compare(otherSignatureString) == 0 ) {
		return true;
	}

	return false;
}

void Chip::setSignatureString(const std::string &newChipSignatureString) {
	signatureString = new std::string(newChipSignatureString);
}

void Chip::setChipName(const std::string &newChipName) {
	if (chipName != 0) {
		delete chipName;
		chipName = 0;
	}
	chipName = new std::string( newChipName );
}

void Chip::addMemory(const std::string &name, shared_ptr<Memoria> memPtr) {
	//memoriesMap[name] = memPtr;
	memoriesMap.insert(pair<string,shared_ptr<Memoria>>(name, memPtr));
}

void Chip::addProperty(shared_ptr<Property> property) {
	//TODO Check if an instantiation is really required
	propertiesMap[property->getName()] = property;
}

void Chip::addProgrammerProperty(shared_ptr<Property> progProperty) {
	programmerPropertiesMap[ progProperty->getName() ] = progProperty;
}

void Chip::loadFromFile(Ficheiro &ficheiro, const std::string &memoryName) {
	if ( ficheiro.obtem_modo_ficheiro() == Ficheiro::escrita ) {
		//Reading from file is not supported
		throw InvalidFileException("Ficheiro apenas para modo escrita");
	}

	Memoria *memoria = getMemory(memoryName);
	if (memoria == 0) {
		throw InvalidFileException( "Memória não encontrada (" + memoryName + ")." );
	}

	memoria->le_ficheiro( &ficheiro );
	int res = memoria->obtem_resultado_oper();
	if ( res != Memoria::ok ) {
		throw InvalidFileException( "Erro na leitura" );
	}
}

void Chip::writeToFile(const std::string &memoryName, Ficheiro &ficheiro) {
	if ( ficheiro.obtem_modo_ficheiro() == Ficheiro::leitura ) {
		//Writing to file is not supported
		throw InvalidFileException("Ficheiro apenas para modo escrita");
	}

	Memoria *memoria = getMemory(memoryName);
	if (memoria == 0) {
		throw InvalidFileException( "Memória não encontrada (" + memoryName + ")." );
	}

	memoria->escreve_ficheiro( &ficheiro );
	int res = memoria->obtem_resultado_oper();
	if ( res != Memoria::ok ) {
		throw InvalidFileException( "Erro na escrita" );
	}
}
