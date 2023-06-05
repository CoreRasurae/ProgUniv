/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ProgrammerFactory.hh
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

#ifndef PROGRAMMERFACTORY_HH_
#define PROGRAMMERFACTORY_HH_

#include "../include/Programmer.hh"

class ProgrammerFactory {
	public:
		static  ProgrammerFactory& getFactory();
		shared_ptr<Programmer> createProgrammer(string prog_name, string manufacturer_name, string chip_ref);
		
	private:
		ProgrammerFactory();
		ProgrammerFactory(ProgrammerFactory const&); //Esconder construtor por cópia
		ProgrammerFactory& operator= (ProgrammerFactory const&); //Operador de atribuição escondido
    		
		~ProgrammerFactory();
		
		static ProgrammerFactory* factory;
};


/*template <class ProgrammerClass>
class ProgrammerFactoryInstantiator {
	public:
		ProgrammerClass *create();
		~ProgrammerFactoryInstantiator() {}
};

template <class ProgrammerClass>
ProgrammerClass *ProgrammerFactoryInstantiator<ProgrammerClass>::create() {
	return new ProgrammerClass;
}*/

#endif /*PROGRAMMERFACTORY_HH_*/
