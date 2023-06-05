/*
 * ChipOptionsFileRWManager.hh
 *
 *  Created on: Aug 21, 2013
 *      Author: lpnm
 */

#include <memory>
#include "../include/ChipIF.hh"

#ifndef CHIPOPTIONSFILERWMANAGER_H_
#define CHIPOPTIONSFILERWMANAGER_H_

class ChipOptionsFileRWManager {
public:
	ChipOptionsFileRWManager();
	virtual ~ChipOptionsFileRWManager();
	std::shared_ptr<std::list<std::pair<std::string,std::string>>> readOptions(std::string filename);
	void writeOptions(std::list<std::pair<std::string,std::string>> &options, std::string filename);
	void readOptions(InterfaceClass::ChipIF &chip, string filename);
	void writeOptions(InterfaceClass::ChipIF &chip, std::string filename);
};

#endif /* CHIPOPTIONSFILERWMANAGER_H_ */
