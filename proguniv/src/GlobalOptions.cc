/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            GlobalOptions.cc
 *
 *  Copyright  2021  Luís Mendes
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
 
#include <algorithm>
#include <iostream>
#include "../include/GlobalOptions.hh"

const std::string GlobalOptions::HAS_EXTERNAL_CLOCK_SOURCE = "HasExtClockSource";

GlobalOptions *GlobalOptions::instance = 0;
 
GlobalOptions::GlobalOptions() {
    knownKeys.insert(HAS_EXTERNAL_CLOCK_SOURCE);
}

GlobalOptions& GlobalOptions::getInstance() {
    if (instance == 0)
        instance = new GlobalOptions();
    return *instance;
}

void GlobalOptions::addOptions(std::list<std::pair<std::string, std::string>> options) {
    auto iter = options.begin();
    for (; iter != options.end(); iter++) {
        std::pair<std::string, std::string> option = *iter;
        std::unordered_set<std::string>::const_iterator resIter = knownKeys.find(option.first);
        if (resIter == knownKeys.end()) {
            //throw
        }
        std::transform(option.second.begin(), option.second.end(),option.second.begin(), ::toupper);
        std::cout << "Inserting global option: " << option.first << " with value: " << option.second << std::endl;       
        knownOptions.insert(option);
    }
}

std::string GlobalOptions::getOption(std::string optionKey) {
    std::unordered_map<std::string,std::string>::const_iterator resIter = knownOptions.find(optionKey);
    if (resIter == knownOptions.end()) {
        return {};
    }
    return resIter->second;
}
