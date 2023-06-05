/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *             ApolloV.hh
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
  
#ifndef APOLLOV_HH_
#define APOLLOV_HH_

#include "../include/Programmer.hh"
#include "../include/ATMELGenericSerialHwIF.hh"
#include "../include/LPTInterfaceControl.hh"
#include "../include/InvalidAddressException.hh"

/**
 * ApolloV - Atmel Apollo V Programmer
 */ 
class ApolloV :  public Programmer, public virtual InterfaceClass::ATMELGenericSerialHwIF  {
	public:
		ApolloV();
		
		virtual ~ApolloV();
		
		static shared_ptr<Programmer> create(string prog_name, string manufacturer_name, string chip_ref);
		
	protected:
		std::shared_ptr<Programmer> genericAtmelProgrammer;
		int lptBaseAddress; 
		bool initialized; //Flags that indicates if the programmer has been correctly initialized in order to be used
	
		void attachVirtualProgrammer(std::shared_ptr<Programmer> programmer);
		
		//M�todos da interface ATMELGenericSerialIF
	 	virtual void setMOSI(unsigned char value);
	 		
	 	virtual unsigned char getMISO();
	 		
	 	virtual void setSCK(unsigned char value);	
	 		
	 	virtual void setPOWER(unsigned char value);
	 		
	 	virtual void setRESET(unsigned char value);

 		virtual void setCLK(unsigned char value);
 		
		//M�todos da classe Programmer
		virtual bool checkChipSignature(bool *isLocked = 0);

		virtual void writeMemory(const std::string &memoryId, const bool verifyWhileWrite, const bool isChipErased);
		
		virtual void readMemory(const std::string &memoryId);
		
		virtual void eraseMemory(const std::string &memoryId);
		
		virtual void writeConfiguration();
		
		virtual void readConfiguration();
		
		virtual void eraseConfiguration();
		
		virtual void eraseAll();
		
		virtual InterfaceClass::ChipIF *getChip();
		
		virtual void setParameter(const std::string &parameter, const std::string &value);
};

#endif /* APOLLOV_HH_ */
