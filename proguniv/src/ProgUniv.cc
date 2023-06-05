/***************************************************************************
 *            ProgUniv.cc
 *
 *  Copyright  2007  Luís Mendes
 *  Email
 ****************************************************************************/
/*
 * CeCILL v2 FREE SOFTWARE LICENSE AGREEMENT
 *
 *
 *   Notice
 *
 * This Agreement is a Free Software license agreement that is the result
 * of discussions between its authors in order to ensure compliance with
 * the two main principles guiding its drafting:
 *
 *   * firstly, compliance with the principles governing the distribution
 *     of Free Software: access to source code, broad rights granted to
 *     users,
 *   * secondly, the election of a governing law, French law, with which
 *     it is conformant, both as regards the law of torts and
 *     intellectual property law, and the protection that it offers to
 *     both authors and holders of the economic rights over software.
 *
 * The authors of the CeCILL (for Ce[a] C[nrs] I[nria] L[ogiciel] L[ibre])
 * license are:
 *
 * Commissariat à l'Energie Atomique - CEA, a public scientific, technical
 * and industrial research establishment, having its principal place of
 * business at 25 rue Leblanc, immeuble Le Ponant D, 75015 Paris, France.
 *
 * Centre National de la Recherche Scientifique - CNRS, a public scientific
 * and technological establishment, having its principal place of business
 * at 3 rue Michel-Ange, 75794 Paris cedex 16, France.
 *
 * Institut National de Recherche en Informatique et en Automatique -
 * INRIA, a public scientific and technological establishment, having its
 * principal place of business at Domaine de Voluceau, Rocquencourt, BP
 * 105, 78153 Le Chesnay cedex, France.
 *
 */

/*#include "hardinterface.hh"*/
/*#include "programador.hh"*/
//#include "stdafx.h"
#include <unistd.h>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include "../include/GlobalOptions.hh"
#include "../include/Chip.hh"
#include "../include/Memoria.hh"
#include "../include/Ficheiro.hh"
#include "../include/Intelhex.hh"
#include "../include/Intelhex16.hh"
#include "../include/LPTInterfaceControl.hh"
#include "../include/TimerIF.hh"
#include "../include/Programmer.hh"
#include "../include/ProgrammerFactory.hh"
#include "../include/ChipIF.hh"
#include "../include/ATMELGenericSerialHwIF.hh"
#include "../include/ApolloV.hh"
#include "../include/RealTime.hh"
#include "../include/ChipOptionsFileRWManager.hh"
/* #include "../include/pic16f874.hh" */
/*#include "microprocessador.hh"*/
/*#include "temporizador.hh"*/

void converte8M_16() {
  Memoria mem, mem2;
  Intelhex fich, fich2, fich3;
  Intelhex16 fich16, fich16_2;
  int res;

  //Leitura intelhex16
  mem.configura(0x1000, 0x0000, 14, "User Memory");
  if (mem.obtem_resultado_oper() != Memoria::ok) {
		cout << "erro a configurar memoria temp" << endl;
		exit(1);
	}

  mem.fill_value(0x0000, 0x0000, 0x1000);

  mem2.configura(0x1000, 0x0000, 14, "User Memory");
  if (mem2.obtem_resultado_oper() != Memoria::ok) {
		cout << "erro a configurar memoria temp" << endl;
		exit(1);
	}

  mem2.fill_value(0x0000, 0x0000, 0x1000);

 fich.abre("PicProgUpdate 1v01.HEX", Ficheiro::leitura);
  if (fich.obtem_resultado_oper() != Ficheiro::ok) {
  	cout << "Erro na leitura do ficheiro" << endl;
  	exit(1);
  }

  mem.le_ficheiro(&fich);
  if (mem.obtem_resultado_oper() != Memoria::ok) {
  	cout << "Erro na escrita do ficheiro" << endl;
  	exit(1);
  }

  fich16.abre("PicProgUpdate_1v01_16.hex", Ficheiro::escrita);
  if (fich16.obtem_resultado_oper() != Ficheiro::ok) {
  	cout << "Erro na leitura do ficheiro" << endl;
  	exit(1);
  }

  mem.escreve_ficheiro(&fich16);
  if (mem.obtem_resultado_oper() != Memoria::ok) {
  	cout << "Erro na escrita do ficheiro" << endl;
  	exit(1);
  }

  fich16.fecha();

  fich16_2.abre("PicProgUpdate_1v01_16.hex", Ficheiro::leitura);
  if (fich16_2.obtem_resultado_oper() != Ficheiro::ok) {
  	cout << "Erro na leitura do ficheiro" << endl;
  	exit(1);
  }

  mem2.le_ficheiro(&fich16_2);
  if (mem2.obtem_resultado_oper() != Memoria::ok) {
  	cout << "Erro na escrita do ficheiro" << endl;
  	exit(1);
  }

  fich16_2.fecha();

  res = mem.compara(&mem2);
  if (mem.obtem_resultado_oper() != Memoria::ok) {
  	cout << "Erro ao comparar memória" << endl;
  	exit(1);
  }

  if (!res) {
  	cout << "Memórias diferentes" << endl;
  }

  fich.fecha();
}

void do_fusesUVBox(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	//No internal clock source
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0110");
	//Internal Clock Source
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Enabled"); //1Mhz
	//chip->setConfigurationProperty("FUSELOW_CKDIV8", "Enabled"); //8Mhz
	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void do_fusesUSBGateway(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	//No internal clock source
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0110");
	//Internal Clock Source
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");
	//External Clock Source (new Gateway Hardware 06/12/2011)
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0000");
	//chip->setExternalClockFrequency(1000000);

	//With bodlevel
	//chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "1V8");
	//chip->setConfigurationProperty("FUSELOW_SUT", "00");

	//Without BodLevel
	chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "Disabled");
	chip->setConfigurationProperty("FUSELOW_SUT", "10");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaUSBGateway(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("knx_eib_gateway.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fichEeprom.abre("knx_eib_gateway.ee.hex", Ficheiro::leitura);
	if (fichEeprom.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA328P");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());
	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");
	chip->loadFromFile(fichEeprom, "Data");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesUSBGateway(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);
	prog.writeMemory("Data", true, true);
	//prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	/*prog.eraseMemory("");
	  */

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);

	/*fich2.abre(nome_hex, ficheiro::leitura);
	if (fich2.obtem_resultado_oper() != ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
	  	exit(1);
	}

	  pic.le_ficheiro(&fich2);
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro a carregar memória a partir do ficheiro" << endl;
	  	exit(1);
	  }

	  //pic.apaga();


	  config_bits = 0;
	  config_bits |= pic16f874::CP0;
	  config_bits |= pic16f874::CP1;
	  config_bits |= pic16f874::CPD; // Memórias desprotegidas
	  config_bits |= pic16f874::BODEN; //Brown-out reset
	  //config_bits |= pic16f874::LVP; // Low Voltage Programming activado
	  config_bits |= pic16f874::WRT;
	  config_bits |= pic16f874::HS;

	  cout << "Palavra de configuração: " << hex << config_bits << dec << endl;

	  pic.carregaConfiguracao(config_bits);
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro a carregar configuração" << endl;
	  	exit(1);
	  }

	  pic.programaConfiguracao();
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro a programar configuração" << endl;
	  	exit(1);
	  }

	  pic.programa(true);
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro na programação do pic" << endl;
	  	exit(1);
	  }

	  pic.le();
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro na leitura do conteúdo do pic" << endl;
	  	exit(1);
	  }

	  fich.abre("res.hex", ficheiro::escrita);
	  pic.escreve_ficheiro(&fich);
	    if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro na escrita do conteúdo do pic para ficheiro" << endl;
	  	exit(1);
	  }

	  fich.fecha();

	  if (!pic.compara_ficheiro(&fich2)) {
	  	cout << "ERRO: conteúdo do ficheiro original é diferente do lido do pic" << endl;
	  	exit(1);
	}*/
}

void programaUVBox(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("uvbox.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fichEeprom.abre("uvbox.ee.hex", Ficheiro::leitura);
	if (fichEeprom.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATTINY461");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());
	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");
	chip->loadFromFile(fichEeprom, "Data");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesUVBox(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.writeMemory("Data", true, true);
	//prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	/*prog.eraseMemory("");
	  */

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void do_fusesLambda(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	//Select clock source to internal calibrated RC Oscillator - 8.0Mhz (for fast programming)
	chip->setConfigurationProperty("LOCKBYTE_LOCKBIT", "Mode1");
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT1", "Mode1");
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT0", "Mode1");
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "1110");

	prog.writeConfiguration();

	printf("Here\n");
	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);
	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaLambda(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("V0.0.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA8");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());
	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseConfiguration();
	prog.eraseAll();
	do_fusesLambda(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	/*prog.eraseMemory("");
	  */

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void do_fusesBootloader(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	chip->setConfigurationProperty("LOCKBYTE_LOCKBIT", "Mode1");
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT1", "Mode3");
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT0", "Mode1");
	chip->setConfigurationProperty("FUSEHIGH_CKOPT", "High");
	chip->setConfigurationProperty("FUSEHIGH_BOOTSZ", "1024w");
	chip->setConfigurationProperty("FUSEHIGH_BOOTRST", "BootVector");
	//chip->setConfigurationProperty("FUSEHIGH_BOOTRST", "Vector0000");
	chip->setConfigurationProperty("FUSELOW_BODLEVEL", "2V7");
	//Select clock source to internal calibrated RC Oscillator - 8.0Mhz (for fast programming)
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0100");

	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);
	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaBootloader(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("bootloader_atmega8.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA8");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());
	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseConfiguration();
	prog.eraseAll();
	do_fusesBootloader(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	/*prog.eraseMemory("");
	  */

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void do_fusesBootloader168(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	chip->setExternalClockFrequency(0);
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT1", "Mode3");

	chip->setConfigurationProperty("FUSEEXT_BOOTSZ", "1024w");
	chip->setConfigurationProperty("FUSEEXT_BOOTRST", "BootVector");

	chip->setConfigurationProperty("FUSEHIGH_BODLEVEL", "Disabled");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	//Select clock source to internal calibrated RC Oscillator - 8.0Mhz
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");
	//Select low power crystal clock source up to 16MHz
	chip->setConfigurationProperty("FUSELOW_CKSEL", "1110");
	chip->setConfigurationProperty("FUSELOW_SUT", "10");

	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaBootloader168(char *nome_hex)  {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("bootloader.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA168");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());

	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesBootloader168(prog);
	prog.readConfiguration();
	cout << "Detected target board clock frequency: " << prog.getTargetBoardClockFrequency() << endl;


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);

}

void do_fusesBootloader168_Spansion(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	chip->setExternalClockFrequency(0);
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT1", "Mode3");

	chip->setConfigurationProperty("FUSEEXT_BOOTSZ", "1024w");
	chip->setConfigurationProperty("FUSEEXT_BOOTRST", "BootVector");

	chip->setConfigurationProperty("FUSEHIGH_BODLEVEL", "Disabled");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	//Select clock source to internal calibrated RC Oscillator - 8.0Mhz
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");
	chip->setConfigurationProperty("FUSELOW_SUT", "10");

	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaBootloader168_Spansion(char *nome_hex)  {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("bootloader168_spansion.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA168PA");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());

	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesBootloader168_Spansion(prog);
	prog.readConfiguration();
	cout << "Detected target board clock frequency: " << prog.getTargetBoardClockFrequency() << endl;


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);

}

void do_fusesBootloader328_Spansion(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	chip->setExternalClockFrequency(0);
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT1", "Mode3");

	chip->setConfigurationProperty("FUSEHIGH_BOOTSZ", "1024w");
	chip->setConfigurationProperty("FUSEHIGH_BOOTRST", "BootVector");

	chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "1V8");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	//Select clock source to internal calibrated RC Oscillator - 8.0Mhz
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");
	chip->setConfigurationProperty("FUSELOW_SUT", "10");

	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}


void programaBootloader328_Spansion(char *nome_hex)  {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("bootloader328_spansion.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA328P");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());

	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesBootloader328_Spansion(prog);
	prog.readConfiguration();
	cout << "Detected target board clock frequency: " << prog.getTargetBoardClockFrequency() << endl;


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);

}

void do_fusesRadiationCounter(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	chip->setConfigurationProperty("FUSEEXT_BOOTSZ", "1024w");
	chip->setConfigurationProperty("FUSEHIGH_BODLEVEL", "4V3");
	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	//Select clock source to internal calibrated RC Oscillator - 8.0Mhz
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");
	chip->setConfigurationProperty("FUSELOW_SUT", "10");

	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}


void programaRadiationCounter(char *nome_hex)  {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("MEGA88COUNTER.HEX", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA168");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());

	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesRadiationCounter(prog);
	prog.readConfiguration();
	cout << "Detected target board clock frequency: " << prog.getTargetBoardClockFrequency() << endl;


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);

}

void do_fusesOlimexinoBootloader(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	chip->setExternalClockFrequency(16000000);
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT1", "Mode2");
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT0", "Mode1");
	chip->setConfigurationProperty("LOCKBYTE_LOCKBIT", "Mode1");

	chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "2V6");
	chip->setConfigurationProperty("FUSEEXT_HWBE", "Enabled");

	chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Disabled");
	chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");

	chip->setConfigurationProperty("FUSEHIGH_BOOTSZ", "2048w");
	chip->setConfigurationProperty("FUSEHIGH_BOOTRST", "BootVector");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	//Select low power crystal clock source up to 16MHz
	chip->setConfigurationProperty("FUSELOW_CKSEL", "1111");
	chip->setConfigurationProperty("FUSELOW_SUT", "11");

	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaOlimexinoBootloader(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("Caterina-Leonardo.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA32U4");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());

	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesOlimexinoBootloader(prog);
	prog.readConfiguration();
	cout << "Detected target board clock frequency: " << prog.getTargetBoardClockFrequency() << endl;


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void do_fusesOlimexinoBootloaderDFU(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
	   	return;
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	chip->setExternalClockFrequency(16000000);
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT1", "Mode2");
	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT0", "Mode1");
	chip->setConfigurationProperty("LOCKBYTE_LOCKBIT", "Mode1");

	chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "2V6");
	chip->setConfigurationProperty("FUSEEXT_HWBE", "Enabled");

	chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Disabled");
	chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");

	chip->setConfigurationProperty("FUSEHIGH_BOOTSZ", "2048w");
	chip->setConfigurationProperty("FUSEHIGH_BOOTRST", "Vector0000");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	//Select low power crystal clock source up to 16MHz
	chip->setConfigurationProperty("FUSELOW_CKSEL", "1111");
	chip->setConfigurationProperty("FUSELOW_SUT", "11");

	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada Calibration Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada Lock Bit Byte\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaOlimexinoBootloaderDFU(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("BootloaderDFU_Olimexino32U4.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA32U4");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());

	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.eraseAll();
	prog.readConfiguration();
	do_fusesOlimexinoBootloaderDFU(prog);
	prog.readConfiguration();
	cout << "Detected target board clock frequency: " << prog.getTargetBoardClockFrequency() << endl;


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void do_fusesATMega8Reset(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	//Internal Clock Source
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");

	//Without BodLevel
	chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "4V3");
	chip->setConfigurationProperty("FUSELOW_SUT", "10");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Enabled");
	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaATMega8Reset(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("ATMega8ParallelReset.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fichEeprom.abre("ATMega8ParallelReset.ee.hex", Ficheiro::leitura);
	if (fichEeprom.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA328P");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());
	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");
	chip->loadFromFile(fichEeprom, "Data");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesATMega8Reset(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);
	prog.writeMemory("Data", true, true);
	//prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	/*prog.eraseMemory("");
	  */

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void do_fusesAtTinyClockGen(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	//No internal clock source
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0110");
	//Internal Clock Source
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled"); //8Mhz
	//chip->setConfigurationProperty("FUSELOW_CKDIV8", "Enabled"); //1Mhz
	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaAtTinyClockGen(char *nome_hex) {
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("clockgen.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATTINY861");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());
	prog.setParameter("LPTBaseAddress","0x378");
	//prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");
	chip->loadFromFile(fichEeprom, "Data");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesAtTinyClockGen(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);

	prog.writeMemory("Data", true, true);
	//prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	/*prog.eraseMemory("");
	  */

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void do_fusesUSBGatewayV1(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	//No internal clock source
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0110");
	//Internal Clock Source
	chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");
	//External Clock Source (new Gateway Hardware 06/12/2011)
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0000");
	//chip->setExternalClockFrequency(1000000);

	//With bodlevel
	//chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "1V8");
	//chip->setConfigurationProperty("FUSELOW_SUT", "00");

	//Without BodLevel
	chip->setConfigurationProperty("FUSEHIGH_BODLEVEL", "Disabled");
	chip->setConfigurationProperty("FUSELOW_SUT", "10");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaUSBGatewayV1(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("knx_eib_gateway.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fichEeprom.abre("knx_eib_gateway.ee.hex", Ficheiro::leitura);
	if (fichEeprom.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA168");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());
	//prog.setParameter("LPTBaseAddress","0x378");
	//prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");
	chip->loadFromFile(fichEeprom, "Data");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesUSBGatewayV1(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);
	prog.writeMemory("Data", true, true);
	//prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();

	/*prog.eraseMemory("");
	  */

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);

	/*fich2.abre(nome_hex, ficheiro::leitura);
	if (fich2.obtem_resultado_oper() != ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
	  	exit(1);
	}

	  pic.le_ficheiro(&fich2);
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro a carregar memória a partir do ficheiro" << endl;
	  	exit(1);
	  }

	  //pic.apaga();


	  config_bits = 0;
	  config_bits |= pic16f874::CP0;
	  config_bits |= pic16f874::CP1;
	  config_bits |= pic16f874::CPD; // Memórias desprotegidas
	  config_bits |= pic16f874::BODEN; //Brown-out reset
	  //config_bits |= pic16f874::LVP; // Low Voltage Programming activado
	  config_bits |= pic16f874::WRT;
	  config_bits |= pic16f874::HS;

	  cout << "Palavra de configuração: " << hex << config_bits << dec << endl;

	  pic.carregaConfiguracao(config_bits);
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro a carregar configuração" << endl;
	  	exit(1);
	  }

	  pic.programaConfiguracao();
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro a programar configuração" << endl;
	  	exit(1);
	  }

	  pic.programa(true);
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro na programação do pic" << endl;
	  	exit(1);
	  }

	  pic.le();
	  if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro na leitura do conteúdo do pic" << endl;
	  	exit(1);
	  }

	  fich.abre("res.hex", ficheiro::escrita);
	  pic.escreve_ficheiro(&fich);
	    if (pic.obtem_resultado() != pic16f874::ok) {
	  	cout << "Erro na escrita do conteúdo do pic para ficheiro" << endl;
	  	exit(1);
	  }

	  fich.fecha();

	  if (!pic.compara_ficheiro(&fich2)) {
	  	cout << "ERRO: conteúdo do ficheiro original é diferente do lido do pic" << endl;
	  	exit(1);
	}*/
}

void do_fusesUSBGatewayV2(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	chip->setConfigurationProperty("LOCKBYTE_BOOTLOCKBIT1", "Mode2");

	chip->setConfigurationProperty("FUSEEXT_HWBE", "Enabled");

	//With bodlevel
	chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "3V5");

	chip->setConfigurationProperty("FUSEHIGH_OCDEN", "Disabled");
	chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Disabled");
	chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	chip->setConfigurationProperty("FUSEHIGH_BOOTSZ", "2048w"),
	chip->setConfigurationProperty("FUSEHIGH_EESAVE", "Enabled");
	chip->setConfigurationProperty("FUSEHIGH_BOOTRST", "Vector0000"),

	//External Clock Source
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0000");
	//Internal Clock Source
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");
	//Crystal Clock Source (8Mhz)
	chip->setConfigurationProperty("FUSELOW_CKSEL", "1101");
	chip->setConfigurationProperty("FUSELOW_SUT", "01");
	chip->setExternalClockFrequency(8000000);

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	chip->setConfigurationProperty("FUSELOW_CKOUT", "Disabled");
	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}


void programaUSBGatewayV2(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x5000);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;

	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);

	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "ATMEGA32U4");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());

	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x5000");

	InterfaceClass::ChipIF *chip = prog.getChip();

	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("BootloaderDFU.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fichEeprom.abre("BootloaderDFU.eep", Ficheiro::leitura);
	if (fichEeprom.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	chip->loadFromFile(fich, "Program");
	chip->loadFromFile(fichEeprom, "Data");

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.eraseAll();
	prog.readConfiguration();
	do_fusesUSBGatewayV2(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);
	prog.writeMemory("Data", true, true);
	prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	fichEeprom.fecha();

	realTime.leaveRTMode();


	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void do_fusesCCCPBootloader(Programmer &prog) {
	unsigned int valor;
	InterfaceClass::ChipIF *chip = prog.getChip();

	Memoria *mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
	   	return;
	}
	mem->le(&valor);
	printf("Lock Bit Byte vale: %02x\n", valor);

	//8Mhz Crystal oscillator - No internal clock source
	chip->setConfigurationProperty("FUSELOW_CKSEL", "1101");
	chip->setExternalClockFrequency(8000000);
	//Internal Clock Source
	//chip->setConfigurationProperty("FUSELOW_CKSEL", "0010");

	//With bodlevel
	//chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "1V8");
	//chip->setConfigurationProperty("FUSELOW_SUT", "10");

	//Without BodLevel, fast rising power
	chip->setConfigurationProperty("FUSEEXT_BODLEVEL", "Disabled");
	chip->setConfigurationProperty("FUSELOW_SUT", "10");

	chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Disabled");
	chip->setConfigurationProperty("FUSEHIGH_BOOTSZ", "2048w");

	chip->setConfigurationProperty("FUSELOW_CKDIV8", "Disabled");
	chip->setConfigurationProperty("FUSELOW_CKOUT", "Enabled");
	prog.writeConfiguration();

	mem = chip->getMemory("Fuse Low Byte");
	mem->le(&valor);
	printf("Fuse Low Byte vale: %02x\n", valor);

	prog.readConfiguration();
	printf("Leituras dos Fusíveis\n");
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
	  	printf("Memoria não encontrada\n");
	}
	else {
		mem->le(&valor);
		printf("Byte de calibração vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Low Byte\n");
	}
	else {
		mem->le(&valor);
		printf("Fuse Low Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse High Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse High Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria não encontrada: Fuse Extended Byte\n");
	}
	else {
		mem->le( &valor );
		printf("Fuse Extended Byte vale: %02x\n", valor);
	}

	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}
	mem->le( &valor );
	printf("Lock Bit Byte vale: %02x\n", valor);
}

void programaCCCPBootloader(char *nome_hex) {
	//LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x378);
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(0x3000);

	//Programmer programmer(&tmprz, &lpt);

	Intelhex fich, fichEeprom, fich2, fich3;
	Intelhex16 fich16, fich16_2;
	/*pic16f874 pic;
	unsigned int config_bits = 0;*/

	//pic.associaProgramador(&programmer);
	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	lpt.sendByte(0);
	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	fich.abre("CCCP.hex", Ficheiro::leitura);
	if (fich.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}

	/*fichEeprom.abre("CCCP.ee.hex", Ficheiro::leitura);
	if (fichEeprom.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na leitura do ficheiro" << endl;
		exit(1);
	}*/

	fich2.abre("resultado.hex", Ficheiro::escrita);
	if (fich2.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}

	fich3.abre("dados.hex", Ficheiro::escrita);
	if (fich3.obtem_resultado_oper() != Ficheiro::ok) {
		cout << "Erro na escrita do ficheiro" << endl;
		exit(1);
	}


	shared_ptr<Programmer> progRef = factory.createProgrammer("ApolloV", "ATMEL", "AT90USB1286");
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	Programmer& prog = *(progRef.get());
	//prog.setParameter("LPTBaseAddress","0x378");
	prog.setParameter("LPTBaseAddress","0x3000");

	InterfaceClass::ChipIF *chip = prog.getChip();
	chip->loadFromFile(fich, "Program");
	//chip->loadFromFile(fichEeprom, "Data");

	//chip->setConfigurationProperty("FUSEHIGH_JTAGEN", "Enabled");
	/*try {
		chip->setConfigurationProperty("FUSEHIGH_WDTON", "Disabled");
	} catch ( InvalidPropertyNameException &e ) {
		cout << "Erro não fatal, FUSEHIGH_WDTON não existe: " << e.what();
	}*/

	realTime.enterRTMode();

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	prog.eraseAll();
	do_fusesCCCPBootloader(prog);
	prog.readConfiguration();


	//prog.setParameter("GenerateTestPattern", "true");
	prog.writeMemory("Program", true, true);
	prog.writeMemory("Data", true, true);
	//prog.readMemory("ALL");
	chip->writeToFile("Program", fich2);
	fich2.finaliza_escrita();
	fich2.fecha();

	chip->writeToFile("Data", fich3);
	fich3.finaliza_escrita();
	fich3.fecha();

	fich.fecha();
	//fichEeprom.fecha();

	realTime.leaveRTMode();

	/*prog.eraseMemory("");
	  */

	lpt.sendByte(0);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void programa(char *nome_hex) {
#ifdef __linux__
	InterfaceClass::TimerIF::createTimer("STD");
#else
	InterfaceClass::TimerIF::createTimer("EXT");
#endif
	//programaRadiationCounter(nome_hex);
	//programaLambda(nome_hex);
	//programaBootloader(nome_hex);
	//programaBootloader168(nome_hex);
	//programaBootloader168_Spansion(nome_hex);
	//programaBootloader328_Spansion(nome_hex);
	//programaUSBGateway(nome_hex);
	//programaUVBox(nome_hex);
	//programaATMega8Reset(nome_hex);
	//programaAtTinyClockGen(nome_hex);
	//programaOlimexinoBootloader(nome_hex);
	//programaOlimexinoBootloaderDFU(nome_hex);
	//programaUSBGatewayV1(nome_hex);
	//programaUSBGatewayV2(nome_hex);
	programaCCCPBootloader(nome_hex);
}

void le_conteudo() {
  /*LPTInterfaceControl lpt;
  Timer tmprz;
  tmprz.initTimer();
  lpt.initLPTInterfaceControl(0x378);
  Programmer programmer(&tmprz, &lpt);
  pic16f874 pic;
  intelhex fich, fich2, fich3;

  pic.associaProgramador(&programmer);

  char c;
  cout << "insert the chip and press a key" << endl;
  cin.get(c);

  pic.le();
  if (pic.obtem_resultado() != pic16f874::ok) {
  	cout << "Erro na leitura do conteúdo do pic" << endl;
  	exit(1);
  }

  fich.abre("res.hex", ficheiro::escrita);
  pic.escreve_ficheiro(&fich);
    if (pic.obtem_resultado() != pic16f874::ok) {
  	cout << "Erro na escrita do conteúdo do pic para ficheiro" << endl;
  	exit(1);
  }

  fich.fecha();*/
}

string parse_config_selector_option(int argc, string* argv) {
	string argument = string(argv[0]);
	string option = "--CFG=";
	string value = argument.substr(option.length());

	return value;
}

bool check_config_selector_option(int argc, string* argv) {
	if (argc == 0) {
		return false;
	}

	//Preserve original option case
	string option = "--CFG=";
	string argument = string(argv[0]);
	std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);
    if (argument.find(option, 0) == 0) {
    	return true;
    }

    return false;
}

pair<string,string> parse_option_in_selector_option(int argc, string* argv) {
	string argument = string(argv[0]);
	string option = "--OPTION=";
	string value = argument.substr(option.length());
	int index = value.find(",", 0);
	if (index == 0) {
		//TODO Throw InvalidCommandLineException
	}

	string optionName = value.substr(0, index);
	string optionValue = value.substr(index + 1);

	return pair<string,string>(optionName, optionValue);
}

bool check_option_selector_option(int argc, string* argv) {
    if (argc == 0) {
        return false;
    }

    //Preserve original option case
    string option = "--OPTION=";
    string argument = string(argv[0]);
    std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);
    if (argument.find(option, 0) == 0) {
    	return true;
    }

    return false;
}

pair<string,string> parse_file_in_selector_option(int argc, string* argv) {
	string argument = string(argv[0]);
	string option = "--FILE=";
	string value = argument.substr(option.length());
	int index = value.find(",", 0);
	if (index == 0) {
		//TODO Throw InvalidCommandLineException
	}

	string targetMemory = value.substr(0, index);
	string filename = value.substr(index + 1);

	return pair<string,string>(targetMemory, filename);
}

bool check_file_in_selector_option(int argc, string* argv) {
	if (argc == 0) {
		return false;
	}

	//Preserve original option case
	string option = "--FILE=";
	string argument = string(argv[0]);
	std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);
    if (argument.find(option, 0) == 0) {
    	return true;
    }

    return false;
}

pair<string,string> parse_programmer_selector_option(int argc, string* argv) {
	string argument = string(argv[0]);
	string option = "--PROG=";
	string value = argument.substr(option.length());
	int index = value.find(",", 0);
	if (index == 0) {
		//TODO Throw InvalidCommandLineException
	}

	string manufacturer = value.substr(0, index);
	string factoryNameReference = value.substr(index + 1);

	return pair<string,string>(manufacturer, factoryNameReference);
}

bool check_programmer_selector_option(int argc, string* argv) {
	if (argc == 0) {
		return false;
	}

	//Preserve original option case
	string option = "--PROG=";
	string argument = string(argv[0]);
	std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);
    if (argument.find(option, 0) == 0) {
    	return true;
    }

    return false;
}

pair<string,string> parse_interface_selector_option(int argc, string* argv) {
	string iface;
	string ifaceNameOrAddressOption = "";

	string argument = string(argv[0]);
	string option = "--IFACE=";
	string value = argument.substr(option.length());
	int index = value.find(",", 0);
	if (index == 0) {
		iface = value;
	} else {
		iface = value.substr(0, index);
		ifaceNameOrAddressOption = value.substr(index + 1);
	}

	return pair<string,string>(iface, ifaceNameOrAddressOption);

}

bool check_interface_selector_option(int argc, string* argv) {
	if (argc == 0) {
		return false;
	}

	//Preserve original option case
	string option = "--IFACE=";
	string argument = string(argv[0]);
	std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);
    if (argument.find(option, 0) == 0) {
    	return true;
    }

    return false;
}

pair<string,string> parse_chip_selector_option(int argc, string* argv) {
	string argument = string(argv[0]);
	string option = "--CHIP=";
	string value = argument.substr(option.length());
	int index = value.find(",", 0);
	if (index == 0) {
		//TODO Throw InvalidCommandLineException
	}

	string manufacturer = value.substr(0, index);
	string factoryNameReference = value.substr(index + 1);

	return pair<string,string>(manufacturer, factoryNameReference);
}

bool check_chip_selector_option(int argc, string* argv) {
	if (argc == 0) {
		return false;
	}

	string option = "--CHIP=";

	//Preserve original option case
	string argument = string(argv[0]);
	std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);
	if (argument.find(option, 0) == 0) {
		return true;
	}

	return false;
}

void cmd_program_operation(int argc, string* argv) {
    bool chipSelOption = false;
    bool ifaceSelOption = false;
    bool progSelOption = false;
    bool filesSelOption = false;
    bool cfgSelOption = false;
    pair<string, string> ifaceDetails = pair<string, string>("","");
    pair<string, string> chipDetails = pair<string, string>("","");
    pair<string, string> programmerDetails = pair<string,string>("","");
    list<pair<string,string>> filesIn;
    string configDetails = "";
    list<pair<string,string>> options;

    for (int index=0; index < argc; index++) {
        if (check_interface_selector_option(argc-index, &argv[index])) {
            //Parse interface options
            ifaceDetails = parse_interface_selector_option(argc-index, &argv[index]);
            ifaceSelOption = true;
        }
        if (check_chip_selector_option(argc-index, &argv[index])) {
            chipDetails = parse_chip_selector_option(argc-index, &argv[index]);
            chipSelOption = true;
        }
        if (check_programmer_selector_option(argc-index, &argv[index])) {
            programmerDetails = parse_programmer_selector_option(argc-index, &argv[index]);
            progSelOption = true;
        }
        if (check_file_in_selector_option(argc-index, &argv[index])) {
            pair<string, string> fileIn = parse_file_in_selector_option(argc-index, &argv[index]);
            filesIn.push_back(fileIn);
            filesSelOption = true;
        }
        if (check_config_selector_option(argc-index, &argv[index])) {
            configDetails = parse_config_selector_option(argc-index, &argv[index]);
            cfgSelOption = true;
        }
        if (check_option_selector_option(argc-index, &argv[index])) {
            pair<string, string> option = parse_option_in_selector_option(argc-index, &argv[index]);
            options.push_back(option);
        }
    }

    bool fail = false;
    if (!chipSelOption) {
        cout << "Missing chip selector option" << endl;
        fail = true;
    }
    if (!progSelOption) {
        cout << "Missing programmer selector option" << endl;
        fail = true;
    }
    if (!ifaceSelOption) {
        cout << "Missing interface selector option" << endl;
        fail = true;
    }
    if (!filesSelOption) {
    	cout << "Missing files in selector option(s)" << endl;
    	fail = true;
    }
    GlobalOptions& globalOpt = GlobalOptions::getInstance();
    globalOpt.addOptions(options);

    if (fail) {
    	return;
    }

#ifdef __linux__
	InterfaceClass::TimerIF::createTimer("STD");
#else
	InterfaceClass::TimerIF::createTimer("EXT");
#endif

	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	shared_ptr<Programmer> progRef = factory.createProgrammer(programmerDetails.second, chipDetails.first, chipDetails.second);
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}

	list<pair<string,shared_ptr<Intelhex>>> files;
	auto filesInIter = filesIn.begin();
	for (;filesInIter != filesIn.end(); filesInIter++) {
		pair<string,string> fileDetail = *filesInIter;
		shared_ptr<Intelhex> file = shared_ptr<Intelhex>(new Intelhex());
		file->abre(fileDetail.second, Ficheiro::leitura);
		if (file->obtem_resultado_oper() != Ficheiro::ok) {
			cout << "Erro na leitura do ficheiro" << endl;
			exit(1);
		}
		files.push_back(pair<string,shared_ptr<Intelhex>>(fileDetail.first,file));
	}

	Programmer& prog = *(progRef.get());
	InterfaceClass::ChipIF *chip = prog.getChip();
	auto filesIter = files.begin();
	for (;filesIter != files.end(); filesIter++) {
		pair<string,shared_ptr<Intelhex>> entry = *filesIter;
		chip->loadFromFile(*(entry.second.get()), entry.first);
		entry.second->fecha();
	}

	int lptAddress = 0;
	if (ifaceDetails.first.compare("LPT") == 0) {
		stringstream ss;
		ss << hex;
		ss << ifaceDetails.second;
		if (!(ss >> lptAddress)) {
			cout << "Invalid address format" << endl;
		}

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
		prog.setParameter("LPTBaseAddress",ifaceDetails.second);
	}

	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	if (cfgSelOption) {
		ChipOptionsFileRWManager cofRW;
		cofRW.readOptions(*chip, configDetails);
	}
	prog.writeConfiguration();
	filesIter = files.begin();
	for (;filesIter != files.end(); filesIter++) {
		pair<string,shared_ptr<Intelhex>> entry = *filesIter;
		prog.writeMemory(entry.first, true, true);
	}

	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.enterRTMode();
	}

	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.leaveRTMode();

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
	}

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);

}

void cmd_verify_operation(int argc, string* argv) {
	cout << "Unimplemented yet" << endl;
}

void cmd_read_operation(int argc, string* argv) {
	bool chipSelOption = false;
	bool ifaceSelOption = false;
	bool progSelOption = false;
	bool filesSelOption = false;
	bool cfgSelOption = false;
	pair<string, string> ifaceDetails = pair<string, string>("","");
	pair<string, string> chipDetails = pair<string, string>("","");
	pair<string, string> programmerDetails = pair<string,string>("","");
	list<pair<string,string>> options;
	list<pair<string,string>> filesIn;
	string configDetails = "";

	for (int index=0; index < argc; index++) {
		if (check_interface_selector_option(argc-index, &argv[index])) {
			//Parse interface options
			ifaceDetails = parse_interface_selector_option(argc-index, &argv[index]);
			ifaceSelOption = true;
		}
		if (check_chip_selector_option(argc-index, &argv[index])) {
			chipDetails = parse_chip_selector_option(argc-index, &argv[index]);
			chipSelOption = true;
		}
		if (check_programmer_selector_option(argc-index, &argv[index])) {
			programmerDetails = parse_programmer_selector_option(argc-index, &argv[index]);
			progSelOption = true;
		}
		if (check_file_in_selector_option(argc-index, &argv[index])) {
			pair<string, string> fileIn = parse_file_in_selector_option(argc-index, &argv[index]);
			filesIn.push_back(fileIn);
			filesSelOption = true;
		}
		if (check_config_selector_option(argc-index, &argv[index])) {
			configDetails = parse_config_selector_option(argc-index, &argv[index]);
			cfgSelOption = true;
		}
	        if (check_option_selector_option(argc-index, &argv[index])) {
			pair<string, string> option = parse_option_in_selector_option(argc-index, &argv[index]);
			options.push_back(option);
		}
	}

	bool fail = false;
	if (!chipSelOption) {
		cout << "Missing chip selector option" << endl;
		fail = true;
	}
	if (!progSelOption) {
		cout << "Missing programmer selector option" << endl;
		fail = true;
	}
	if (!ifaceSelOption) {
		cout << "Missing interface selector option" << endl;
		fail = true;
	}
    if (!filesSelOption) {
    	cout << "Missing files in selector option(s)" << endl;
    	fail = true;
    }
    GlobalOptions& globalOpt = GlobalOptions::getInstance();
    globalOpt.addOptions(options);


    if (fail) {
    	return;
    }

#ifdef __linux__
	InterfaceClass::TimerIF::createTimer("STD");
#else
	InterfaceClass::TimerIF::createTimer("EXT");
#endif

	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	shared_ptr<Programmer> progRef = factory.createProgrammer(programmerDetails.second, chipDetails.first, chipDetails.second);
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}

	list<pair<string,shared_ptr<Intelhex>>> files;
	auto filesInIter = filesIn.begin();
	for (;filesInIter != filesIn.end(); filesInIter++) {
		pair<string,string> fileDetail = *filesInIter;
		shared_ptr<Intelhex> file = shared_ptr<Intelhex>(new Intelhex());
		file->abre(fileDetail.second, Ficheiro::escrita);
		if (file->obtem_resultado_oper() != Ficheiro::ok) {
			cout << "Erro na leitura do ficheiro" << endl;
			exit(1);
		}
		files.push_back(pair<string,shared_ptr<Intelhex>>(fileDetail.first,file));
	}

	Programmer& prog = *(progRef.get());
	InterfaceClass::ChipIF *chip = prog.getChip();

	int lptAddress = 0;
	if (ifaceDetails.first.compare("LPT") == 0) {
		stringstream ss;
		ss << hex;
		ss << ifaceDetails.second;
		if (!(ss >> lptAddress)) {
			cout << "Invalid address format" << endl;
		}

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
		prog.setParameter("LPTBaseAddress",ifaceDetails.second);
	}

	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.enterRTMode();
	}

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	if (cfgSelOption) {
		ChipOptionsFileRWManager cofRW;
		cofRW.writeOptions(*chip, configDetails);
	}

	auto filesIter = files.begin();
	for (;filesIter != files.end(); filesIter++) {
		pair<string,shared_ptr<Intelhex>> entry = *filesIter;
		prog.readMemory(entry.first);
	}

	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.leaveRTMode();

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
	}

	filesIter = files.begin();
	for (;filesIter != files.end(); filesIter++) {
		pair<string,shared_ptr<Intelhex>> entry = *filesIter;
		chip->writeToFile(entry.first, *(entry.second.get()));
		entry.second->finaliza_escrita();
		entry.second->fecha();
	}


	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void cmd_erase_operation(int argc, string* argv) {
	bool chipSelOption = false;
	bool ifaceSelOption = false;
	bool progSelOption = false;
	pair<string,string> ifaceDetails = pair<string,string>("","");
	pair<string,string> chipDetails=pair<string,string>("","");
	pair<string,string> programmerDetails=pair<string,string>("","");

	for (int index=0; index < argc; index++) {
		if (check_interface_selector_option(argc-index, &argv[index])) {
			//Parse interface options
			ifaceDetails = parse_interface_selector_option(argc-index, &argv[index]);
			ifaceSelOption = true;
		}
		if (check_chip_selector_option(argc-index, &argv[index])) {
			chipDetails = parse_chip_selector_option(argc-index, &argv[index]);
			chipSelOption = true;
		}
		if (check_programmer_selector_option(argc-index, &argv[index])) {
			programmerDetails = parse_programmer_selector_option(argc-index, &argv[index]);
			progSelOption = true;
		}
	}

	bool fail = false;
	if (!chipSelOption) {
		cout << "Missing chip selector option" << endl;
		fail = true;
	}
	if (!progSelOption) {
		cout << "Missing programmer selector option" << endl;
		fail = true;
	}
	if (!ifaceSelOption) {
		cout << "Missing interface selector option" << endl;
		fail = true;
	}

	if (fail) {
		return;
	}

#ifdef __linux__
	InterfaceClass::TimerIF::createTimer("STD");
#else
	InterfaceClass::TimerIF::createTimer("EXT");
#endif

	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	shared_ptr<Programmer> progRef = factory.createProgrammer(programmerDetails.second, chipDetails.first, chipDetails.second);
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	int lptAddress = 0;
	Programmer& prog = *(progRef.get());
	if (ifaceDetails.first.compare("LPT") == 0) {
		stringstream ss;
		ss << hex;
		ss << ifaceDetails.second;
		if (!(ss >> lptAddress)) {
			cout << "Invalid address format" << endl;
		}

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
		prog.setParameter("LPTBaseAddress",ifaceDetails.second);
	}

	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	InterfaceClass::ChipIF *chip = prog.getChip();
	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.enterRTMode();
	}

	bool isLocked = false;
	if ( prog.checkChipSignature(&isLocked) == false && !isLocked ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.eraseAll();

	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.leaveRTMode();

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
	}

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void cmd_read_config_operation(int argc, string* argv) {
	bool chipSelOption = false;
	bool ifaceSelOption = false;
	bool progSelOption = false;
	bool cfgSelOption = false;
	pair<string,string> ifaceDetails = pair<string,string>("","");
	pair<string,string> chipDetails=pair<string,string>("","");
	pair<string,string> programmerDetails=pair<string,string>("","");
	string configDetails = "";

	for (int index=0; index < argc; index++) {
		if (check_interface_selector_option(argc-index, &argv[index])) {
			//Parse interface options
			ifaceDetails = parse_interface_selector_option(argc-index, &argv[index]);
			ifaceSelOption = true;
		}
		if (check_chip_selector_option(argc-index, &argv[index])) {
			chipDetails = parse_chip_selector_option(argc-index, &argv[index]);
			chipSelOption = true;
		}
		if (check_programmer_selector_option(argc-index, &argv[index])) {
			programmerDetails = parse_programmer_selector_option(argc-index, &argv[index]);
			progSelOption = true;
		}
		if (check_config_selector_option(argc-index, &argv[index])) {
			configDetails = parse_config_selector_option(argc-index, &argv[index]);
			cfgSelOption = true;
		}
	}
	bool fail = false;
	if (!chipSelOption) {
		cout << "Missing chip selector option" << endl;
		fail = true;
	}
	if (!progSelOption) {
		cout << "Missing programmer selector option" << endl;
		fail = true;
	}
	if (!ifaceSelOption) {
		cout << "Missing interface selector option" << endl;
		fail = true;
	}
	if (!cfgSelOption) {
		cout << "Missing configuration selector option" << endl;
		fail = true;
	}

	if (fail) {
		return;
	}

#ifdef __linux__
	InterfaceClass::TimerIF::createTimer("STD");
#else
	InterfaceClass::TimerIF::createTimer("EXT");
#endif

	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	shared_ptr<Programmer> progRef = factory.createProgrammer(programmerDetails.second, chipDetails.first, chipDetails.second);
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	int lptAddress = 0;
	Programmer& prog = *(progRef.get());
	if (ifaceDetails.first.compare("LPT") == 0) {
		stringstream ss;
		ss << hex;
		ss << ifaceDetails.second;
		if (!(ss >> lptAddress)) {
			cout << "Invalid address format" << endl;
		}

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
		prog.setParameter("LPTBaseAddress",ifaceDetails.second);
	}

	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	InterfaceClass::ChipIF *chip = prog.getChip();
	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.enterRTMode();
	}

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();

	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.leaveRTMode();

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
	}

	ChipOptionsFileRWManager cofRW;
	cofRW.writeOptions(*chip, configDetails);

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void cmd_write_config_operation(int argc, string* argv) {
	bool chipSelOption = false;
	bool ifaceSelOption = false;
	bool progSelOption = false;
	bool cfgSelOption = false;
	pair<string,string> ifaceDetails = pair<string,string>("","");
	pair<string,string> chipDetails=pair<string,string>("","");
	pair<string,string> programmerDetails=pair<string,string>("","");
	string configDetails = "";

	for (int index=0; index < argc; index++) {
		if (check_interface_selector_option(argc-index, &argv[index])) {
			//Parse interface options
			ifaceDetails = parse_interface_selector_option(argc-index, &argv[index]);
			ifaceSelOption = true;
		}
		if (check_chip_selector_option(argc-index, &argv[index])) {
			chipDetails = parse_chip_selector_option(argc-index, &argv[index]);
			chipSelOption = true;
		}
		if (check_programmer_selector_option(argc-index, &argv[index])) {
			programmerDetails = parse_programmer_selector_option(argc-index, &argv[index]);
			progSelOption = true;
		}
		if (check_config_selector_option(argc-index, &argv[index])) {
			configDetails = parse_config_selector_option(argc-index, &argv[index]);
			cfgSelOption = true;
		}
	}
	bool fail = false;
	if (!chipSelOption) {
		cout << "Missing chip selector option" << endl;
		fail = true;
	}
	if (!progSelOption) {
		cout << "Missing programmer selector option" << endl;
		fail = true;
	}
	if (!ifaceSelOption) {
		cout << "Missing interface selector option" << endl;
		fail = true;
	}
	if (!cfgSelOption) {
		cout << "Missing configuration selector option" << endl;
		fail = true;
	}

	if (fail) {
		return;
	}

#ifdef __linux__
	InterfaceClass::TimerIF::createTimer("STD");
#else
	InterfaceClass::TimerIF::createTimer("EXT");
#endif

	ProgrammerFactory& factory = ProgrammerFactory::getFactory();
	RealTime& realTime = RealTime::getInstance();

	shared_ptr<Programmer> progRef = factory.createProgrammer(programmerDetails.second, chipDetails.first, chipDetails.second);
	if (progRef == 0) {
		cout << "Programmer not built." << endl;
		exit(1);
	}
	int lptAddress = 0;
	Programmer& prog = *(progRef.get());
	if (ifaceDetails.first.compare("LPT") == 0) {
		stringstream ss;
		ss << hex;
		ss << ifaceDetails.second;
		if (!(ss >> lptAddress)) {
			cout << "Invalid address format" << endl;
		}

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
		prog.setParameter("LPTBaseAddress",ifaceDetails.second);
	}

	char c;
	cout << "insert chip and press a key" << endl;
	cin.get(c);

	InterfaceClass::ChipIF *chip = prog.getChip();
	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.enterRTMode();
	}

	if ( prog.checkChipSignature() == false ) {
		cout << "Assinatura do Chip não corresponde ao chip esperado (" << chip->getChipName() << ")." << endl;
		exit(1);
	}

	prog.readConfiguration();
	ChipOptionsFileRWManager cofRW;
	cofRW.readOptions(*chip, configDetails);
	prog.writeConfiguration();

	if (ifaceDetails.first.compare("LPT") == 0) {
		realTime.leaveRTMode();

		LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptAddress);
		lpt.sendByte(0);
	}

	cout << "Programming finished!!! Please remove the chip or ICSP cable and press enter." << endl;
	cin.get(c);
}

void cmd_list_chips(int argc, string* argv) {
	string chipsList = Chip::listKnownChips();

	cout << chipsList << endl;
}

void cmd_list_configurations(int argc, string* argv) {
	if (!check_chip_selector_option(argc, argv)) {
		cout << "Missing chip selector option" << endl;
		return;
	}

	pair<string,string> chip = parse_chip_selector_option(argc, argv);
	cout << "Manufacturer: " + chip.first + ", Chip: " + chip.second << endl;
	shared_ptr<InterfaceClass::ChipIF> chipIf = InterfaceClass::ChipIF::createInstance(chip.first, chip.second);

	string result = "Available Properties: \n";
	auto propList = chipIf->getPropertyList();
	auto propListIterator = propList->begin();
	for (; !propList->empty() && propListIterator != propList->end(); propListIterator++) {
		string property = *propListIterator;
		if (!property.empty()) {
			result += "      " + property + "\n";
		}
	}

	cout << result << endl;
}

void cmd_list_configuration(int argc, string* argv) {
	if (!check_chip_selector_option(argc, argv)) {
		cout << "Missing chip selector option" << endl;
		return;
	}

	if (argc != 2) {
		return;
	}

	pair<string,string> chip = parse_chip_selector_option(argc, argv);
	cout << "Manufacturer: " + chip.first + ", Chip: " + chip.second << endl;
	shared_ptr<InterfaceClass::ChipIF> chipIf = InterfaceClass::ChipIF::createInstance(chip.first, chip.second);

	//try {
	const Property& property = (Property &)chipIf->getConfigurationProperty(argv[1]);
	//TODO catch - ensure exception is thrown if property not found

	string result = "Property: " + argv[1] + "\n";
	shared_ptr<list<string>> acceptedValuesList = property.getAcceptedValuesList();
	result += "Description: " + property.getDescription() + "\n";
	auto valuesIterator = acceptedValuesList->begin();
	for (; !acceptedValuesList->empty() && valuesIterator != acceptedValuesList->end(); valuesIterator++) {
		string value = *valuesIterator;
		result += "     Accepted value: " + value + "\n";
		result += "     Detail:         " + property.getAcceptedValueDetail(value) + "\n";
	}

	cout << result << endl;
}

unique_ptr<string[]> convertToStringArray(int argc, char**argv) {
	unique_ptr<string[]> result = std::unique_ptr<string[]>(new string[argc]);
	for (int index = 0; index < argc; index++) {
		result[index] = string(argv[index]);
	}

	return std::move(result);
}

void cmd_help() {
    cout << "### ProgUniv command line help ###" << endl << endl;
    cout << "ProgUniv <command> <command options>" << endl << endl;
    cout << "Commands listing:" << endl;
    cout << "--PROGRAM <interface> <programmer> <chip> <--no-verify>? <files in options>" << endl;
    cout << "--VERIFY <interface> <programmer> <chip> <files in options>" << endl;
    cout << "--ERASE <interface> <programmer> <chip> <memories selector>" << endl;
    cout << "--READ_CONFIGURATIONS <interface> <programmer> <chip> <out cfg file>" << endl;
    cout << "--WRITE_CONFIGURATIONS <interface> <programmer> <chip> <in cfg file>" << endl;
    cout << "--LIST_CHIPS" << endl;
    cout << "--LIST_CONFIGURATIONS <chip>" << endl;
    cout << "--LIST_CONFIGURATION <chip> <configuration name>" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "<interface option>: --IFACE=LPT,0x378 or --IFACE=USB<,/dev/usb/hiddev0>?" << endl;
    cout << "<programmer selector>: --PROG=<Manufacturer>,<Name>" << endl;
    cout << "<chip selector>: --CHIP=<Manufacturer>,<Device name>" << endl;
    cout << "<files in options>: --FILE=<Memory name>,<file name.hex>" << endl;
    cout << "<files out options>: --FILE=<Memory name>,<file name.hex>" << endl;
    cout << "<in cfg file>/<out cfg file>: --CFG=fuses.cfg" << endl;
    cout << endl;
    cout << "Extra options:" << endl;
    cout << "<options selector>: --OPTION=<option key>,<option value>" << endl;
    cout << endl;
    cout << "Example:" << endl;
    cout << "ProgUniv --PROGRAM --IFACE=LPT,0x378 --PROG=Default,ApolloV \\" << endl;
    cout << " --CHIP=ATMEL,ATMEGA328P --FILE=PROGRAM,program.hex \\" <<endl;
    cout << " --FILE=DATA,data.hex --CFG=fuses.cfg --OPTION=HasExtClockSource,True" << endl;
}

void command_line_parser(int argc, char** argv) {
	unique_ptr<string[]> commandOptionsPtr = 0;
	for (int iter = 1; iter < argc; iter++) {
		string argument = string(argv[iter]);

		if (iter == 1) {
			std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);
			cout << argument << endl;
			commandOptionsPtr = convertToStringArray(argc-2, &argv[2]);
			string *commandOptions = commandOptionsPtr.get();
			if (argument.compare("--PROGRAM") == 0) {
				cmd_program_operation(argc - 2, commandOptions);
			} else if (argument.compare("--VERIFY") == 0) {
				cmd_verify_operation(argc - 2, commandOptions);
			} else if (argument.compare("--READ") == 0) {
				cmd_read_operation(argc - 2, commandOptions);
			} else if (argument.compare("--ERASE") == 0) {
				cmd_erase_operation(argc - 2, commandOptions);
			} else if (argument.compare("--READ_CONFIGURATIONS") == 0) {
				cmd_read_config_operation(argc - 2, commandOptions);
			} else if (argument.compare("--WRITE_CONFIGURATIONS") == 0) {
				cmd_write_config_operation(argc - 2, commandOptions);
			} else if (argument.compare("--LIST_CHIPS") == 0) {
				cmd_list_chips(argc - 2, commandOptions);
			} else if (argument.compare("--LIST_CONFIGURATIONS") == 0) {
				cmd_list_configurations(argc - 2, commandOptions);
			} else if (argument.compare("--LIST_CONFIGURATION") == 0) {
				cmd_list_configuration(argc - 2, commandOptions);
			} else {
				cmd_help();
			}
		}
	}

	if (argc == 1) {
		cmd_help();
	}
}

int main(int argc, char **argv) {
	//le_conteudo();
	//programa("bootloader.hex");
	command_line_parser(argc, argv);
}
