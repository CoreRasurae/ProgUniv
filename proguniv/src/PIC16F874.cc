/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            PIC16F874.cc
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

//TODO portar código antigo para a nova arquitectura do ProgUniv

#include "../include/PIC16F874.hh"
#include "../include/TimerIF.hh"
#include <iostream>
#include <bitset>

using std::ios;
using namespace std;

PIC16F874::PIC16F874()
{
	program_mem.configura(0x1000, 0x0000, 14, "User Memory");
	program_mem.fill_value(0x0000, 0x0000, 0x1000);
	configuration_mem.configura(0x0010, 0x2000, 14, "Configuration Memory");
	configuration_mem.fill_value(0x3fff, 0x2000, 0x0010);
	data_mem.configura(0x0080, 0x2100, 8, "Data Memory");
	data_mem.fill_value(0xff, 0x2100, 0x0080);
	programador = 0;
    resultado_oper = ok;
}

PIC16F874::~PIC16F874()
{
}



void PIC16F874::le_ficheiro(Ficheiro *fich) {
	if (fich == 0)
		resultado_oper = erro;
		
	program_mem.le_ficheiro(fich);
	if (program_mem.obtem_resultado_oper() != Memoria::ok) {
		resultado_oper = erro;
		return;
	}
	
	configuration_mem.le_ficheiro(fich);
	if (configuration_mem.obtem_resultado_oper() != Memoria::ok) {
		resultado_oper = erro;
		return;
	}
	
	//data_mem.le_ficheiro(fich);
	if (data_mem.obtem_resultado_oper() != Memoria::ok) {
		resultado_oper = erro;
		return;
	}
	
	resultado_oper = ok;
}

void PIC16F874::escreve_ficheiro(Ficheiro *fich) {
	if (fich == 0)
		resultado_oper = erro;
		
	program_mem.escreve_ficheiro(fich);
	if (program_mem.obtem_resultado_oper() != Memoria::ok) {
		resultado_oper = erro;
		return;
	}
	
	configuration_mem.escreve_ficheiro(fich);
	if (configuration_mem.obtem_resultado_oper() != Memoria::ok) {
		resultado_oper = erro;
		return;
	}

	//data_mem.escreve_ficheiro(fich);
	if (data_mem.obtem_resultado_oper() != Memoria::ok) {
		resultado_oper = erro;
		return;
	}

	resultado_oper = ok;		
}

int PIC16F874::compara_ficheiro(Ficheiro *fich) {
	Memoria temp_mem;
	
	if (fich == 0) {
		resultado_oper = erro;
		return 0;
	}

    temp_mem.configura(0x1000, 0x0000, 14, "User Memory");
	if (temp_mem.obtem_resultado_oper() != Memoria::ok) {
		cout << "erro a configurar memoria temp" << endl;
		resultado_oper = erro;
		return 0;
	}

	temp_mem.fill_value(0x0000, 0x0000, 0x1000);

	temp_mem.le_ficheiro(fich);
	if (temp_mem.obtem_resultado_oper() != Memoria::ok) {
		cout << "erro a ler do ficheiro" << endl;
		resultado_oper = erro;
		return 0;
	}

	return program_mem.compara(&temp_mem);
}
	

void PIC16F874::carregaConfiguracao(unsigned int config) {
	config |= RESV;
	
	configuration_mem.escreve(0x2007, config);
	if (configuration_mem.obtem_resultado_oper() != Memoria::ok) {
		resultado_oper = erro;
		return;
	}
	
	resultado_oper = ok;
}

bool PIC16F874::confirmaVersao() {
	resultado_oper = ok;
	unsigned int deviceID = 0;
	int i;
	bool res = true;
	
	if (programador == 0) {
		resultado_oper = erro;
		return false;
	}

/*
	programador->enterLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		programador->leaveLVProgrammingMode();
		resultado_oper = erro;
		return false;
	}

	programador->loadConfiguration(bitset<14>(string("11111111111111")).to_ulong());
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
	}

	if (resultado_oper == ok) {	
		for (i = 0; i < 6; i++) {
			programador->incrementAddress();
			if (programador->obtemResultadoOper() != Programador::ok) {
				resultado_oper = erro;
				break;
			}
		}

		if (resultado_oper == ok) {
			deviceID = programador->readDataProgramMemory();
			if ((deviceID >> 5) != bitset<9>(string("001001001")).to_ulong()) 
				res = false;
		}
	}
	
	programador->leaveLVProgrammingMode();
	*/
	
	return res;
}

//Cuidado: Não utilizar, a menos que o pic já esteja estragado
bool PIC16F874::programaVersao() {
	resultado_oper = ok;
	unsigned int deviceID = 0;
	int i;
	bool res = true;
	
	if (programador == 0) {
		resultado_oper = erro;
		return false;
	}

/*
	programador->enterLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		programador->leaveLVProgrammingMode();
		resultado_oper = erro;
		return false;
	}

	programador->loadConfiguration(bitset<14>(string("11111111111111")).to_ulong());
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
	}

	if (resultado_oper == ok) {	
		for (i = 0; i < 6; i++) {
			programador->incrementAddress();
			if (programador->obtemResultadoOper() != Programador::ok) {
				resultado_oper = erro;
				break;
			}
		}

		programador->loadDataProgramMemory(bitset<14>(string("00100100100100")).to_ulong());
		programador->beginErase();
		programador->delay(8000000);
		
		if (resultado_oper == ok) {
			deviceID = programador->readDataProgramMemory();
			if ((deviceID >> 5) != bitset<9>(string("001001001")).to_ulong()) 
				res = false;
		}
	}
	
	programador->leaveLVProgrammingMode();
	*/
	
	return res;
}


void PIC16F874::programaConfiguracao() {
	resultado_oper = ok;
	unsigned int config, config_lida;
	int i;
	
	if (programador == 0) {
		resultado_oper = erro;
		return;
	}

	/*if (!confirmaVersao()) {
		resultado_oper = chip_incorrecto;
		return;
	}*/

	configuration_mem.le(0x2007, &config);
	if (configuration_mem.obtem_resultado_oper() != Memoria::ok) {
		resultado_oper = erro;
		return;
	}

	/*programador->enterLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		programador->leaveLVProgrammingMode();
		resultado_oper = erro;
		return;
	}

	programador->loadConfiguration(bitset<14>(string("11111111111111")).to_ulong());
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
	}

	if (resultado_oper == ok) {	
		for (i = 0; i < 7; i++) {
			programador->incrementAddress();
			if (programador->obtemResultadoOper() != Programador::ok) {
				resultado_oper = erro;
				break;
			}
		}
		
		programador->loadDataProgramMemory(config);
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
		}

		if (resultado_oper == ok) {
			programador->beginErase();
			//programador->beginProgramming();
			if (programador->obtemResultadoOper() != Programador::ok) {
				resultado_oper = erro;
			}

			programador->delay(8000000); //tera + tprog = 8ms
		}
		
		if (resultado_oper == ok) {
			config_lida = programador->readDataProgramMemory();
			if (programador->obtemResultadoOper() != Programador::ok) {
				resultado_oper = erro;
			}
			
			if (resultado_oper == ok) {
				config |= UNIMPL;
				if (config_lida != config) {
					cout << "Palavra de configuração esperada: " << hex <<
						config << ", obtida: " << config_lida << dec << endl;
					//resultado_oper = erro;
				}
			}
		}
	}
	
	programador->leaveLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}
	*/
}

void PIC16F874::programa(bool fullEraseFirst) {
	unsigned int endereco;
	unsigned int palavra_escrita, palavra_lida;
	
	resultado_oper = ok;
	
	if (programador == 0) {
		resultado_oper = erro;
		return;
	}

	if (!confirmaVersao()) {
		resultado_oper = chip_incorrecto;
		cout << "versao incorrect do chip" << endl;
		return;
	}

	if (fullEraseFirst) {
		this->apaga();
	}

	/*
	programador->enterLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		programador->leaveLVProgrammingMode();
		resultado_oper = erro;
		return;
	}

	for (endereco = 0; endereco < 0x1000; endereco++) {
		program_mem.le(endereco, &palavra_escrita);
		if (program_mem.obtem_resultado_oper() != Memoria::ok) {
			resultado_oper = erro;
			break;
		}
		
		programador->loadDataProgramMemory(palavra_escrita);
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
			break;
		}

		if (fullEraseFirst) {
			programador->beginProgramming();
			if (programador->obtemResultadoOper() != Programador::ok) {
				resultado_oper = erro;
				break;
			}
			
			programador->delay(4050000); //tprog = 4ms
		}
		else {	
			programador->beginErase();
			if (programador->obtemResultadoOper() != Programador::ok) {
				resultado_oper = erro;
				break;
			}
	
	
			programador->delay(8050000); //tera + tprog = 8ms
		}
		
		palavra_lida = programador->readDataProgramMemory();
		if (palavra_lida != palavra_escrita) {
			cout << "Palavra lida = " << hex << palavra_lida <<
				" palavra escrita = " << palavra_escrita << " endereco "
				<< endereco << dec << endl;
			//resultado_oper = erro_programacao_programa;
			//break;
		}
		
		
		programador->incrementAddress();
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
			break;
		}
	}

	programador->leaveLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}
	*/
}

void PIC16F874::verifica() {
	if (programador == 0) {
		resultado_oper = erro;
		return;
	}

	if (!confirmaVersao()) {
		resultado_oper = chip_incorrecto;
		return;
	}

	resultado_oper = ok;
}

void PIC16F874::le() {
	/*int endereco = 0x0000;
	int valor = 0;
	
	if (programador == 0) {
		resultado_oper = erro;
		return;
	}

	if (!confirmaVersao()) {
		resultado_oper = chip_incorrecto;
		return;
	}

	programador->enterLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}
	
	for (endereco = 0; endereco < 0x1000; endereco++) {
		cout << "endereco: " << endereco << endl;
		valor = programador->readDataProgramMemory();
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
			return;
		}
		
		programador->incrementAddress();
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
			return;
		}
		
		program_mem.escreve(endereco, valor);
	}
	
	programador->leaveLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}

	programador->enterLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}

	programador->loadConfiguration(bitset<14>(string("11111111111111")).to_ulong());
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}
	
	for (endereco = 0x2000; endereco < 0x2010; endereco++) {
		cout << "endereco: " << endereco << endl;
		valor = programador->readDataProgramMemory();
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
			return;
		}
		
		programador->incrementAddress();
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
			return;
		}
		
		configuration_mem.escreve(endereco, valor);
		if (configuration_mem.obtem_resultado_oper() != Memoria::ok) {
			resultado_oper = erro;
			return;
		}
	}

	programador->leaveLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}
	
	resultado_oper = ok;
	*/
}

void PIC16F874::apaga() {
	int i;
	int resultado_oper = ok;
	
	/*programador->enterLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}

	programador->loadConfiguration(bitset<14>(string("11111111111111")).to_ulong());
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}

	if (resultado_oper == ok) {	
		for (i = 0; i < 7; i++) {
			programador->incrementAddress();
			if (programador->obtemResultadoOper() != Programador::ok) {
				resultado_oper = erro;
				break;
			}
		}

		programador->bulkErase1();
		programador->bulkErase2();
		
		programador->beginErase();
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
		}

		programador->delay(8000000); //tera + tprog = 8ms
		
 		programador->bulkErase1();
		programador->bulkErase2();
	}
	
	programador->leaveLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}*/
	
	/* não comentado originalmente
	programador->enterLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}
	
	//Apagar memoria nao protegida
	programador->loadDataProgramMemory(bitset<14>(string("11111111111111")).to_ulong());
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}

	if (resultado_oper == ok) {	
		programador->bulkErase1();
		programador->bulkErase2();
		
		programador->beginErase();
		if (programador->obtemResultadoOper() != Programador::ok) {
			resultado_oper = erro;
		}

		programador->delay(8000000); //tera + tprog = 8ms
		
 		programador->bulkErase1();
		programador->bulkErase2();
	}
	
	programador->leaveLVProgrammingMode();
	if (programador->obtemResultadoOper() != Programador::ok) {
		resultado_oper = erro;
		return;
	}
	*/
}

/*void PIC16F874::associaProgramador(Programador *programador) {
	if (programador == 0) {
		resultado_oper = erro;
		return;
	}
	
	this->programador = programador;
}*/

int PIC16F874::obtem_resultado() {
	return resultado_oper;
}

