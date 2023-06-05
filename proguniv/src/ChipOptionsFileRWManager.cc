/*
 * ChipOptionsFileRWManager.cc
 *
 *  Created on: Aug 21, 2013
 *      Author: lpnm
 */

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../include/ChipOptionsFileRWManager.hh"
/*
 * Format is a two column name, value pair per line
 * name is the chip configuration option
 * value is the chip configuration selected value
 */
using namespace std;

ChipOptionsFileRWManager::ChipOptionsFileRWManager() {


}

ChipOptionsFileRWManager::~ChipOptionsFileRWManager() {

}

std::shared_ptr<std::list<std::pair<std::string, std::string> > > ChipOptionsFileRWManager::readOptions(
		std::string filename) {
	std::shared_ptr<list<pair<string,string>>> result =
			shared_ptr<list<pair<string,string>>>(new list<pair<string,string>>());

	ifstream ifs(filename);
	while (ifs) {
		string line;
		std::getline(ifs, line);
		if (line.empty()) {
			continue;
		}
		int index = line.find(",", 0);
		if (index == 0) {
			//TODO Throw InvalidFileFormatException
		}

		string optionName = line.substr(0, index);
		string optionValue = line.substr(index + 1);
		result->push_back(pair<string,string>(optionName,optionValue));
	}
	ifs.close();

	return result;
}

void ChipOptionsFileRWManager::readOptions(InterfaceClass::ChipIF &chip, string filename) {
	shared_ptr<list<pair<string,string>>> optionsList = readOptions(filename);
    auto iter = optionsList->begin();
    for (;!optionsList->empty() && iter != optionsList->end(); iter++) {
    	pair<string, string> &entry = *iter;
    	chip.setConfigurationProperty(entry.first, entry.second);
    }
}

void ChipOptionsFileRWManager::writeOptions(
		std::list<std::pair<std::string, std::string> >& optionsList,
		std::string filename) {
	 fstream file(filename, ios::out);
	 auto iter = optionsList.begin();
	 for (;!optionsList.empty() && iter != optionsList.end(); iter++) {
		 pair<string,string> entry = *iter;
		 file << entry.first << "," << entry.second << endl;
	 }
	 file.close();
}

void ChipOptionsFileRWManager::writeOptions(InterfaceClass::ChipIF& chip,
		std::string filename) {
	list<std::pair<string,string>> optionsList;
	std::shared_ptr<InterfaceClass::ChipIF::propertiesIndexList_t> properties = chip.getPropertyList();
	list<string>::iterator iter = properties->begin();
	for (;!properties->empty() && iter != properties->end(); iter++) {
		string property = *iter;
		if (!property.empty()) {
			const Property& prop = chip.getConfigurationProperty(property);
			string name = prop.getName();
			string value = prop.getValue();
			optionsList.push_back(pair<string,string>(name, value));
		}
	}

	writeOptions(optionsList, filename);
}
