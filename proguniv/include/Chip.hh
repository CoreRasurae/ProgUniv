/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            Chip.hh
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

#ifndef CHIP_HH_
#define CHIP_HH_

#include <map>
#include <string>
#include <list>
#include <tr1/functional>
#include <mutex>

#include "../include/ChipIF.hh"
#include "../include/InvalidPropertyNameException.hh"
#include "../include/Property.hh"

/**
 * Generic Chip Class behaviour -- default implementation
 * Should not be included by the final application
 * Every Chip must inherit from this class
 * This class must not be directly instantiated
 */
class Chip : public virtual InterfaceClass::ChipIF {
	public:
	    static void initFactoryMaps();
	    static string listKnownChips();
		virtual const std::string &getChipName();
		virtual std::unique_ptr<memoriesIndexList_t> getMemoriesList();
		virtual Memoria *getMemory(const std::string &memoryName) const;

		virtual std::unique_ptr<programmerPropertiesIndexList_t> getProgrammerPropertyList();
		virtual bool hasProgrammerConfigurationProperty(const std::string &programmerPropertyName);
		virtual Property &getProgrammerConfigurationProperty(const std::string &programmerPropertyName);

		virtual std::shared_ptr<propertiesIndexList_t> getPropertyList();
		virtual void setConfigurationProperty(const std::string &propertyName, const std::string &propertyValue);
		virtual const Property &getConfigurationProperty(const std::string &propertyName);
		virtual bool checkSignature(const std::string &otherSignatureString);
		virtual void loadFromFile(Ficheiro &ficheiro, const std::string &memoryName);
		virtual void writeToFile(const std::string &memoryName, Ficheiro &ficheiro);
		virtual bool compareWithFile(const char* filename) = 0;
		virtual bool compareWithChip(ChipIF& otherChip) = 0;

		/**
		 * Set's the string with the human readable form chip name
		 * @param newChipName the new name of the chip
		 */
		void setChipName(const std::string &newChipName);

		/**
		 * Set's the chip signature string
		 * @param newChipSignatureString the new chip signature string
		 */
		void setSignatureString(const std::string &newChipSignatureString);

		/**
		 * Adds a new chip memory to the memories map
		 * @param name the new memory name
		 * @param memPtr the memory instance to add
		 */
		void addMemory(const std::string& name, shared_ptr<Memoria> memPtr);

		/**
		 * Adds a new property
		 * @param property the new property to add
		 */
		void addProperty(shared_ptr<Property> property);

		/**
		 * Adds a new programmer property along with it's default value
		 * @param progProperty the new programmer property to add
		 */
		void addProgrammerProperty(shared_ptr<Property> progProperty);

		virtual ~Chip();

		Chip();

	private:
		typedef std::map<std::string, shared_ptr<Property>> programmerPropertiesMap_t;
		typedef std::map<std::string, shared_ptr<Property>> propertiesMap_t;
		typedef std::map<std::string, shared_ptr<Memoria>> memoriesMap_t;
		programmerPropertiesMap_t programmerPropertiesMap;
		propertiesMap_t propertiesMap;
		memoriesMap_t memoriesMap;
		std::string *chipName;
		std::string *signatureString;
};

namespace staticChip {
	static std::map<std::string, std::shared_ptr<std::map<std::string,tr1::function<InterfaceClass::ChipIF*(std::string)>>>> factoryMethodsMaps;
	static std::once_flag factoryMethodsMapsOnceFlag;
}

#endif /*CHIP_HH_*/
