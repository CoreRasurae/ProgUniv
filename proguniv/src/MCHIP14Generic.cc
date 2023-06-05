/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *  
 *            MCHIP14Generic.cc
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

#include "../include/MCHIP14Generic.hh"

/*MCHIP14Generic::MCHIP14Generic() {
	leaveLVProgrammingMode();
}

void MCHIP14Generic::sendCommand(char command) {
	int i;
	char bit;
	
	
	if (!in_program_mode) {
		cout << "Erro: sendCommand - Modo de programação não está activo" << endl;
	resultado_oper = erro;
		return;
	}
	
	for (i = 0; i < 6; i++) {
		bit = command & 0x01;
		command >>= 1;
		
		lpt->setDataBit(DATA_OUT, !bit);
		lpt->setDataBit(CLOCK, !1);
		tmprz->delay(2000); //1us - tdly2, 3.5ms - Delay minimum (Linux timer)
		lpt->setDataBit(CLOCK, !0);
		tmprz->delay(2000); //1us - tdly2, 3.5ms - Delay minimum (Linux timer)
	}
	tmprz->delay(2000); //1us - tdly1 e tdly2
	
	lpt->setDataBit(DATA_OUT, 0);
	
	resultado_oper = ok;
}

void MCHIP14Generic::sendArgument(int arg) {
	int i;
	char bit;

	if (!in_program_mode) {
		cout << "Erro: sendCommand - Modo de programação não está activo" << endl;
	resultado_oper = erro;
		return;
	}
	
	arg &= 0x3fff;
	arg <<= 1;
	
	for (i = 0; i < 16; i++) {
		bit = arg & 0x01;
		arg >>= 1;
		
		lpt->setDataBit(DATA_OUT, !bit);
		lpt->setDataBit(CLOCK, !1);
		tmprz->delay(2000); //1us - tdly2, 3.5ms - Delay minimum (Linux timer)
		lpt->setDataBit(CLOCK, !0);
		tmprz->delay(2000); //1us - tdly2, 3.5ms - Delay minimum (Linux timer)
	}
	tmprz->delay(2000); //1us - tdly1 e tdly2

	lpt->setDataBit(DATA_OUT, 0);
	
	resultado_oper = ok;
}

int MCHIP14Generic::readResult() {
	int result = 0;
	int i;

	if (!in_program_mode) {
		cout << "Erro: sendCommand - Modo de programação não está activo" << endl;
	resultado_oper = erro;
		return result;
	}

	lpt->setDataBit(TriDATA_OUT, 1);
	lpt->setDataBit(DATA_OUT, 0);	
	for (i = 0; i < 16; i++) {
		lpt->setDataBit(CLOCK, !1);
		tmprz->delay(2000); //1us - tdly2 e tdly3, 3.5ms - Delay minimum (Linux timer)
		lpt->setDataBit(CLOCK, !0);
		result |= ((!(lpt->readACK() & 0x01)) << i);
		tmprz->delay(2000); //1us - tdly2, 3.5ms - Delay minimum (Linux timer)
	}
	lpt->setDataBit(TriDATA_OUT, 0);
	tmprz->delay(2000); //1us - tdly1 e tdly2
	
	result >>= 1; //Descartar start bit
	result &= 0x3fff;
	
	resultado_oper = ok;
	return result;
}

void MCHIP14Generic::enterLVProgrammingMode() {
	if (in_program_mode) {
		cout << "Erro: Modo de programação já estava activo" << endl;
	resultado_oper = erro;
		return;
	}
	
	lpt->setDataBit(PGM, !0);
	lpt->setDataBit(VTestMode, 0);
	lpt->setDataBit(CLOCK, !0);
	lpt->setDataBit(DATA_OUT, !0);
	lpt->setDataBit(TriDATA_OUT, 0);
	tmprz->delay(1000); //tset0
	lpt->setDataBit(PGM, !1);
	tmprz->delay(1000); //tset2
	lpt->setDataBit(VTestMode, 1);
	tmprz->delay(6000); //thld0 = 5us, epera 6us
	
	in_program_mode = true;
	
	resultado_oper = ok;
}

void MCHIP14Generic::leaveLVProgrammingMode() {
	lpt->setDataBit(VTestMode, 0);
	lpt->setDataBit(PGM, !0);
	tmprz->delay(1000); //1us - 3.5ms - Delay minimum (Linux timer)
	lpt->setDataBit(CLOCK, 0);
	lpt->setDataBit(DATA_OUT, 0);
	lpt->setDataBit(TriDATA_OUT, 1);

	in_program_mode = false;
	
	resultado_oper = ok;
}
*/
	/*
	* Enters Configuration Address Space - 0x2000
	*/
/*
void MCHIP14Generic::loadConfiguration(int arg) {
	unsigned char command = bitset<8>(string("00000000")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return;
	}
	
	sendArgument(arg);
	if (resultado_oper != ok) {
		return;
	}
	
}
*/

/*
	* Loads Data for Program Memory
	*/
/*
void MCHIP14Generic::loadDataProgramMemory(int arg) {
	unsigned char command = bitset<8>(string("00000010")).to_ulong();	
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return;
	}
	
	
	sendArgument(arg);
	if (resultado_oper != ok) {
		return;
	}
}
*/

/*
	* Reads Data from Program Memory
	*/
/*
int MCHIP14Generic::readDataProgramMemory() {
	unsigned char command = bitset<8>(string("00000100")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return 0;
	}
	
	
	return readResult();
	if (resultado_oper != ok) {
		return 0;
	}
}
*/

/*
	* Loads Data for Data Memory
	*/
	/*
void MCHIP14Generic::loadDataDataMemory(unsigned char arg) {
	unsigned char command = bitset<8>(string("00000011")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return;
	}
	
	
	sendArgument(arg);
	if (resultado_oper != ok) {
		return;
	}
}
*/

   /*
	* Reads Data from Data Memory
	*/
/*
unsigned char MCHIP14Generic::readDataDataMemory() {
	unsigned char temp; 
	unsigned char command = bitset<8>(string("00000101")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return 0;
	}
	
	
	temp = (unsigned char)readResult();
	if (resultado_oper != ok) {
		return 0;
	}
	
	
	return temp;
}
*/

   /*
	* Incrementa Endereço
	*/
/*
void MCHIP14Generic::incrementAddress() { 
	unsigned char command = bitset<8>(string("00000110")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return;
	}
}
*/

   /*
	* Begins Erase/Program Cycle
	*/
/*
void MCHIP14Generic::beginErase() {
	unsigned char command = bitset<8>(string("00001000")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return;
	}
}
*/

/*
	* Begin Programming
	* A Load Command must be given before every begin programming command
	*/
/*
void MCHIP14Generic::beginProgramming() {
	unsigned char command = bitset<8>(string("00011000")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return;
	}
}

void MCHIP14Generic::bulkErase1() {
	unsigned char command = bitset<8>(string("00000001")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return;
	}
}

void MCHIP14Generic::bulkErase2() {
	unsigned char command = bitset<8>(string("00000111")).to_ulong();
	
	sendCommand(command);
	if (resultado_oper != ok) {
		return;
	}
}
*/
