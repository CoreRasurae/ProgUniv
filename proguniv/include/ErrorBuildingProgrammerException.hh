/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *  
 *            ErrorBuildingProgrammerException.hh
 *
 *  Copyright  2007  Luí­s Mendes
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
 
#ifndef ERROR_BUILDING_PROGRAMMER_EXCEPTION_
#define ERROR_BUILDING_PROGRAMMER_EXCEPTION_

#include "CommandException.hh"

class ErrorBuildingProgrammerException : public CommandException 
{
	public:
		ErrorBuildingProgrammerException(const std::string &newMsg) : CommandException(newMsg) {
			
		}
};	

#endif /* ERROR_BUILDING_PROGRAMMER_EXCEPTION_ */
