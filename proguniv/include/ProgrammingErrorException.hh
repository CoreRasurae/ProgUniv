/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *  
 *            ProgrammingErrorException.hh
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
 
#ifndef PROGRAMMING_ERROR_EXCEPTION_
#define PROGRAMMING_ERROR_EXCEPTION_

#include "CommandException.hh"

class ProgrammingErrorException : public CommandException 
{
	public:
		ProgrammingErrorException(const std::string &newMsg) : CommandException(newMsg) {
			
		}
};	

#endif /* PROGRAMMING_ERROR_EXCEPTION_ */
