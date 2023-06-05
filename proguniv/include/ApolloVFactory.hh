/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            ApolloVFactory.hh
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
  
#ifndef APOLLOVFACTORY_HH_
#define APOLLOVFACTORY_HH_

#include "../include/Programmer.hh"
#include "../include/ApolloV.hh"
#include "../include/ProgrammerFactory.hh"

	
/*	class ApolloVFactory : public ProgrammerFactory {
		public:
			Programmer *createProgrammer(const char *prog_name, const char *manufacturer_name, const char *chip_ref);
	};
	
	Programmer *ApolloVFactory::createProgrammer(const char *prog_name, const char *manufacturer_name, const char *chip_ref) {
		ProgrammerFactoryInstantiator<ApolloV> instantiator;
		
		if ( strcmp(prog_name, "ATMEL") ) {
			//throw UnsupportedManufacturerException();
			return 0;
		}
		
		return instantiator.create();
	}*/

#endif /*APOLLOVFACTORY_HH_*/
