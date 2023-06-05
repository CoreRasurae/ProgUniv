/***************************************************************************
 *            ficheiro.hh
 *
 *  Copyright  2021  Luís Mendes
 *  Email
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <list>
#include <string>
#include <unordered_set>
#include <unordered_map>

#ifndef PROGUNIV_INCLUDE_GLOBALOPTIONS_HH_
#define PROGUNIV_INCLUDE_GLOBALOPTIONS_HH_

class GlobalOptions {
public:
    static const std::string HAS_EXTERNAL_CLOCK_SOURCE;

    static GlobalOptions& getInstance();

    void addOptions(std::list<std::pair<std::string, std::string>> options);
    std::string getOption(std::string optionKey);
    bool hasOption(std::string optionName);

private:
    GlobalOptions();
    GlobalOptions(GlobalOptions const&); //Esconder construtor por cópia
    GlobalOptions& operator= (GlobalOptions const&); //Operador de atribuiçãoo escondido

    ~GlobalOptions();

    std::unordered_set<std::string> knownKeys;
    std::unordered_map<std::string, std::string> knownOptions;

    static GlobalOptions *instance;

};

#endif /* PROGUNIV_INCLUDE_GLOBALOPTIONS_HH_ */
