/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *  
 *            MCHIP14Generic.hh
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

#ifndef _MCHIP14_GENERIC_HH_
#define _MCHIP14_GENERIC_HH_

#include "../include/LPTInterfaceControl.hh"
#include "../include/TimerIF.hh"
#include "../include/Programmer.hh"

class MCHIP14Generic : public Programmer {
	static const unsigned int DATA_OUT    = 0; //D0
	static const unsigned int CLOCK       = 1; //D1
	static const unsigned int PGM         = 2; //D2
	static const unsigned int VTestMode   = 3; //D3
	static const unsigned int TriDATA_OUT = 4; //D4

	static const unsigned int DATA_IN   = 6; //ACK - bit ACK

	void sendCommand(char command);
	void sendArgument(int arg);
	
	int readResult();
};

#endif /* _MCHIP14_GENERIC_HH_ */
