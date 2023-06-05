/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            Property.cc
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

#include <string>
#include <memory>
#include <map>
#include <list>
#include "../include/Property.hh"

Property::Property() {
	instance = 0;
	forwardUpdater = 0;
	reverseUpdater = 0;
	readOnly = true;
}

Property::Property(InterfaceClass::ChipIF *instance, const std::string &name, const std::string &value, const std::string &description, bool readOnly) {
	forwardUpdater = 0;
	reverseUpdater = 0;
	
	propertyName = std::string(name);
	propertyValue = std::string(value);
	propertyDescription = std::string(description);
	this->readOnly = readOnly;
	this->instance = instance; 
}

Property::~Property() {
	acceptedValuesMap.clear();
}

void Property::registerForwardUpdater(forwardPtr forwardUpdater) {
	if ( this->forwardUpdater == 0 ) {
		this->forwardUpdater = forwardUpdater;
	}
	
	if ( forwardUpdater != 0 ) {
		//Synchronize internal view with chip
		doForwardUpdate( propertyValue );
	}
}

void Property::registerReverseUpdater(reversePtr reverseUpdater) {
	if ( this->reverseUpdater == 0 ) {
		this->reverseUpdater = reverseUpdater;
	}
}
		
void Property::doForwardUpdate(const std::string &value) {
	if (forwardUpdater == 0)  {
		throw InvalidUpdaterException( "Failure - Forward Updater Function not registered for property name: " + this->getName() );
	}
	
	(*forwardUpdater)(value, *instance);
}

std::string Property::doReverseUpdate() {
	if (reverseUpdater == 0) {
		throw InvalidUpdaterException( "Failure - Reverse Updater Function not registered for property name: " + this->getName() );
	}
	
	std::string result = (*reverseUpdater)(*instance);
	preValidation(result);

	propertyValue = std::string(result);
	
	return result;
}
		
std::string Property::getName() const {
	return propertyName;
}
		
void Property::setValue(const std::string &newValue) {
	if (readOnly)
		throw ReadOnlyPropertyValueException("O valor da propriedade " + propertyName + " é de leitura apenas");
		
	preValidation(newValue);
	
	propertyValue = std::string(newValue);
	if ( forwardUpdater != 0 ) {
		this->doForwardUpdate(newValue);
	}
}

std::string Property::getValue() const {
	return propertyValue;
}
		
std::string Property::getDescription() const {
	return propertyDescription;
}

void Property::addAcceptedValue(const std::string acceptedValue, const std::string detailMsg) {
	acceptedValuesMap[acceptedValue] = detailMsg;
}

std::shared_ptr<acceptedValuesList_t> Property::getAcceptedValuesList() const {
	std::shared_ptr<acceptedValuesList_t> result( new acceptedValuesList_t() );
	valuesMap_t::const_iterator i = acceptedValuesMap.begin();
	
	for (; !acceptedValuesMap.empty() && i != acceptedValuesMap.end(); i++ ) {
		result->push_back((*i).first);
	}
	
	return result;
}

const std::string Property::getAcceptedValueDetail(const std::string &acceptedValue) const {
    std::string detail;
	valuesMap_t::const_iterator i = acceptedValuesMap.find(acceptedValue);
	
	if ( i == acceptedValuesMap.end() ) {
		throw InvalidPropertyValueException("A propriedade " + (propertyName) + " não possui nenhum valor aceite designado " + acceptedValue);
	}
	
	detail = ((*i).second);
	
	return detail;
}

bool Property::isReadOnly() {
	return readOnly;
}

void Property::preValidation(const std::string &value) {
	valuesMap_t::const_iterator i = acceptedValuesMap.find(value);
	
	if ( i == acceptedValuesMap.end() )
		throw InvalidPropertyValueException("A propriedade " + (propertyName) + " não possui nenhum valor aceite designado " + value);
}
