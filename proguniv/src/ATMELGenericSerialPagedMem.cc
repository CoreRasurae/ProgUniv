/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            ATMELGenericSerialPagedMem.cc
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
#include <exception>
#include "../include/ATMELGenericSerialPagedMem.hh"
#include "../include/Utils.hh"
#include "../include/RealTime.hh"
#include "../include/InvalidAddressException.hh"

using namespace std;

ATMELGenericSerialPagedMem::ATMELGenericSerialPagedMem(shared_ptr<InterfaceClass::ATMELGenericSerialHwIF> realProgrammer, shared_ptr<InterfaceClass::ChipIF> targetChip) : ATMELGenericSerial(realProgrammer, targetChip) {

}

ATMELGenericSerialPagedMem::~ATMELGenericSerialPagedMem() {

}

void ATMELGenericSerialPagedMem::writeConfiguration() {
	using InterfaceClass::ATMELGenericSerialHwIF;
	RealTime& realTime = RealTime::getInstance();
	bool realTimeModeOn = false;
	unsigned int valor;
	bool busy;

	Commands cmd;
	CommandArg cmdArg;
	unsigned char readback[4];

	//TODO Don't know if all ATMEL AVR Paged Flash chips have all of these configuration memories
	//TODO Not all fuses can be programmed in serial programming mode, what if the user set's one of those into the memory?

	//Get fuse programming time if available, otherwise use the program flash timing
	unsigned int twd = 6000;
	try {
		try {
			Property &twdProp = chip->getProgrammerConfigurationProperty("minVddTwd_fuse");
			twd = Utils::string2int( twdProp.getValue() );
		} catch (InvalidPropertyNameException &e) {
			Property &twdProp = chip->getProgrammerConfigurationProperty("minVddTwd_flash");
			twd = Utils::string2int( twdProp.getValue() );
		}
	} catch ( exception &e ) {
		throw ProgrammingErrorException( "Unable to obtain specific chip configuration memory programming time, cause: " +
				std::string(e.what()) );
	}

	realTimeModeOn = realTime.getRTMode();
	realTime.enterRTMode();

	serialPowerUpEnterProgramming(true);

	//Write all configuration memories
	Memoria *mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada: Ignoring extended byte fuse memory\n");
	} else {
		resetArgs( cmdArg );
		mem->le( &valor );
		cmdArg.lowDataByteOut = valor;
		cmd = cmdWriteExtendedFuseBits;
		processCommand(cmd, cmdArg, readback, true);

		//Issue command "Poll RDY/_BSY" to check if chip is ready for next command or wait tWD_FLASH before loading next paged
		busy = pollEndOperationWithTimeout(twd);
	}


	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada\n");
		return;
	}

	resetArgs( cmdArg );
	mem->le( &valor );
	cmdArg.lowDataByteOut = valor;
	cmd = cmdWriteFuseHighBits;
	//Force generation of clock signal, in case the previous write changed the clock sources
	processCommand(cmd, cmdArg, readback, true);

	//Issue command "Poll RDY/_BSY" to check if chip is ready for next command or wait tWD_FLASH before loading next paged
	busy = pollEndOperationWithTimeout(twd);


	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada\n");
		return;
	}

	resetArgs( cmdArg );
	mem->le( &valor );
	cmdArg.lowDataByteOut = valor;
	cmd = cmdWriteFuseBits;
	//Force generation of clock signal, in case the previous write changed the clock sources
	processCommand(cmd, cmdArg, readback, true);

	//Issue command "Poll RDY/_BSY" to check if chip is ready for next command
	//or wait tWD_FLASH before loading next paged
	busy = pollEndOperationWithTimeout(twd);



	//The "Lock Bit Byte" must be the last one, otherwise the memories might
	//get locked, including the fuses themselves
	mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada\n");
		return;
	}

	resetArgs( cmdArg );
	mem->le( &valor );
	cmdArg.lowDataByteOut = valor;
	cmd = cmdWriteLockBits;
	//Force generation of clock signal, in case the previous write changed the clock sources
	processCommand(cmd, cmdArg, readback, true);

	//Issue command "Poll RDY/_BSY" to check if chip is ready for next command
	//or wait tWD_FLASH before loading next paged
	busy = pollEndOperationWithTimeout(twd);

	cout << "Chip dispon�vel para novo comando: " << busy << endl;

	serialPowerOff();

	if (!realTimeModeOn)
		realTime.leaveRTMode();
}

void ATMELGenericSerialPagedMem::readConfiguration() {
	using InterfaceClass::ATMELGenericSerialHwIF;
	RealTime& realTime = RealTime::getInstance();
	bool realTimeModeOn = false;

	Commands cmd;
	CommandArg cmdArg;
	unsigned char readback[4];

	realTimeModeOn = realTime.getRTMode();
	realTime.enterRTMode();

	serialPowerUpEnterProgramming(true);

	//Read all configuration memories
	//Prepare to read lock bit byte
	Memoria *mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada\n");
		return;
	}

	resetArgs(cmdArg);
	cmd = cmdReadLockBits;
	//Force generation of clock signal, in case the previous configuration changed the clock sources
	processCommand(cmd, cmdArg, readback, true);
	mem->escreve( readback[3] );


	//Prepare to read fuse extended byte
	mem = chip->getMemory("Fuse Extended Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada: Ignoring fuse extended byte memory\n");
	} else {
		resetArgs(cmdArg);
		cmd = cmdReadExtendedFuseBits;
		//Force generation of clock signal, in case the previous configuration changed the clock sources
		processCommand(cmd, cmdArg, readback, true);
		mem->escreve( readback[3] );
	}


	//Prepare to read fuse high byte
	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada\n");
		return;
	}

	resetArgs(cmdArg);
	cmd = cmdReadFuseHighBits;
	//Force generation of clock signal, in case the previous configuration changed the clock sources
	processCommand(cmd, cmdArg, readback, true);
	mem->escreve( readback[3] );


	//Prepare to read fuse low byte
	mem = chip->getMemory("Fuse Low Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada\n");
		return;
	}

	resetArgs(cmdArg);
	cmd = cmdReadFuseBits;
	//Force generation of clock signal, in case the previous configuration changed the clock sources
	processCommand(cmd, cmdArg, readback, true);
	printf("Li o valor: %02x\n", readback[3]);
	mem->escreve( readback[3] );


	//Prepare to read calibration byte
	mem = chip->getMemory("Calibration Byte");
	if (mem == 0) {
		printf("Memoria n�o encontrada\n");
		return;
	}

	resetArgs(cmdArg);
	cmd = cmdReadCalibrationByte;
	//Force generation of clock signal, in case the previous configuration changed the clock sources
	processCommand(cmd, cmdArg, readback, true);
	mem->escreve( readback[3] );


	serialPowerOff();

	if (!realTimeModeOn)
		realTime.leaveRTMode();
}

void ATMELGenericSerialPagedMem::writeMemoryInternal(const bool verifyWhileWrite, const bool isChipErased, const std::string &memoryId, const bool useTestData) {
	//Dados iniciais
	unsigned int start_address;

	//Var's de estado dos endere�os
	unsigned int current_address = 0;
	unsigned int remaining_data_length = 0;

	//Var's de estado da programa��o das p�ginas
	unsigned int page_data_length;
	unsigned int current_page_index;

	unsigned int dataword, verifyword;

	unsigned int address;

	Commands cmd;
	CommandArg cmdArg;
	unsigned char readback[4];
	bool busy;

	//Generate test pattern destroying memory object information
	if ( useTestData )
		generateTestData( memoryId );

	if (!inProgrammingMode)
		throw NotInProgrammingModeException("O chip n�o se encontra no modo de programa��o");

	Property *pagesProp = 0;
	Property *pageSizeBitsProp = 0;
	unsigned int twd;

	if ( memoryId.compare("Program") == 0 ) {
		pagesProp = &chip->getProgrammerConfigurationProperty("ProgramTotalPages");
		pageSizeBitsProp = &chip->getProgrammerConfigurationProperty("ProgramPageSizeWordsBits");

		//get twd Times
		try {
			Property &prop = chip->getProgrammerConfigurationProperty("minVddTwd_flash");
			twd = Utils::string2int( prop.getValue() );
		} catch ( exception &e ) {
			throw ProgrammingErrorException( "Unable to obtain Chip property: minVddTwd_flash, cause: " + std::string(e.what()) );
		}
	}
	else if ( memoryId.compare("Data") == 0 ) {
		pagesProp = &chip->getProgrammerConfigurationProperty("DataTotalPages");
		pageSizeBitsProp = &chip->getProgrammerConfigurationProperty("DataPageSizeWordsBits");

		//get twd Times
		try {
			Property &prop = chip->getProgrammerConfigurationProperty("minVddTwd_eeprom");
			twd = Utils::string2int( prop.getValue() );
		}
		catch ( exception &e ) {
			throw ProgrammingErrorException( "Unable to obtain Chip property: minVddTwd_eeprom, cause: " + std::string(e.what()) );
		}
	}
	else {
		throw ProgrammingErrorException("Unknown memory type specified: " + memoryId);
	}

	const unsigned int pageSizeBits = Utils::string2int( pageSizeBitsProp->getValue() );
	const unsigned int pageSize = (1 << pageSizeBits);
	const unsigned int pageAddressMask = pageSize - 1;
	cout << "Pagesize: " << pageSize << endl;
	cout << hex << "Pagemask: " << pageAddressMask << dec << endl;

	//Load data
	start_address = 0;

	//Obtain memory data
	Memoria *mem = 0;
	if ( memoryId.compare("Program") == 0 ) {
		mem = chip->getMemory("Program");
		if ( mem == 0 ) {
			printf("Memoria n�o encontrada\n");
			return;
		}
	}
	else {
		mem = chip->getMemory("Data");
		if ( mem == 0 ) {
			printf("Memoria n�o encontrada\n");
			return;
		}
	}

	//Initialize var's
	current_address = start_address; //Must start at the beginning of a page
	remaining_data_length = mem->obtem_tamanho();

	//Get page data length from chip internal configuration
	page_data_length = pageSize;
	if ( remaining_data_length < pageSize ) {
		page_data_length = remaining_data_length;
	}

	unsigned int dataStartOffset;

	bool programPage;

	//Let's go a page at a time, until all data has been programmed/analyzed
	while ( remaining_data_length > 0 ) {
		programPage = false;
		dataStartOffset = 0;

		//Only skip empty positions if the chip is erased or if we are
		//programming the program memory for which the value 0xffff can
		//never be programmed (chip must be erased instead for the latter case)
		if ( isChipErased || memoryId.compare("Program") == 0 ) {
			//Scan the data to see if programming is required for the current page
			for ( current_page_index = (current_address & pageAddressMask);
				  current_page_index < (current_address & pageAddressMask) + page_data_length; current_page_index++ ) {
				address = ( current_address & ~(pageAddressMask) ) | current_page_index;
				if ( ( address >= mem->obtem_tamanho() ) || ( address < 0 ) ) {
					throw InvalidAddressException( "Acesso fora do espa�o de endere�amento" );
				}

				mem->le(address, &dataword);
				if ( memoryId.compare("Program") == 0 ) {
					//0xFFFF Values can never be programmed, either they are already 0xFFFF or else a programming error would occur anyway.
					//Just verify the page and see if there are differences, aborting programming in such cases.
					if ( dataword != 0xffff ) {
						//There is at least one position of memory in the page that must be written
						programPage = true;
						dataStartOffset = address;
						break;
					}
				}
				else {
					//0xFF values can never be programmed, either they are already 0xFF or else a programming error would occur anyway.
					//It is sufficient to ensure that the page on the chip matches the page to be programmed (later in verification step).
					if ( dataword != 0xff ) {
						//There is at least one position of memory in the page that must be written
						programPage = true;
						dataStartOffset = address;
						break;
					}
				}
			}
		}

		if ( programPage ) {
			//Start Programming from the first address with data, not before
			//Current address is now loaded with the effective address of the first byte of the current page
			current_address = dataStartOffset & ~(pageAddressMask);

			//Send a page of data
			//Go through the current page, from the first byte to the last one
			for (current_page_index = (dataStartOffset & pageAddressMask);
				 current_page_index < (current_address & pageAddressMask) + page_data_length; current_page_index++) {
				//Rebuild the full effective address (page + offset), so that data can be loaded from memory
				address = (current_address & ~(pageAddressMask)) | current_page_index;
				if ( ( address >= mem->obtem_tamanho() ) || ( address < 0 ) ) {
					throw InvalidAddressException( "Acesso fora do espa�o de endere�amento" );
				}
				mem->le( address, &dataword );

				if ( memoryId.compare("Program") == 0 ) {
					//Issue Command "Load Program Memory Page" first
					//Load Data low byte before loading data high byte for a given address
					updateExtendedAddressIfRequired(address);
					
					resetArgs(cmdArg);
					cmdArg.highProgramAddress = 0;
					cmdArg.lowProgramAddress = current_page_index;
					cmdArg.lowDataByteOut = (dataword & 0xff);
					cmd = cmdLoadProgramMemoryPageLow;
					processCommand(cmd, cmdArg, readback);

					resetArgs(cmdArg);
					cmdArg.highProgramAddress = 0;
					cmdArg.lowProgramAddress = current_page_index;
					cmdArg.highDataByteOut = (dataword >> 8) & 0xff;
					cmd = cmdLoadProgramMemoryPageHigh;
					processCommand(cmd, cmdArg, readback);
				}
				else {
					//Check if we really have to program the data memory location
					if ( (dataword & 0xff) != readDataAddress( address ) ) {
						//We must program, because old and new values differ
						//Issue Command "Load EEPROM Memory Page"
						cout << "Write... " << current_page_index << endl;
						resetArgs(cmdArg);
						cmdArg.highDataAddress = 0;
						cmdArg.lowDataAddress = current_page_index;
						cmdArg.lowDataByteOut = dataword;
						cmd = cmdLoadEEPROMMemoryPage;
						processCommand(cmd, cmdArg, readback);
					}
				}
			}

			if ( memoryId.compare("Program") == 0 ) {
				//The program memory page is stored with "Write Program memory Page" with the address MSB bits
				updateExtendedAddressIfRequired(current_address);

				resetArgs(cmdArg);
				cmdArg.highProgramAddress = (current_address >> 8);
				cmdArg.lowProgramAddress = (current_address & ~(pageAddressMask));
				cmd = cmdWriteProgramMemoryPage;
				processCommand(cmd, cmdArg, readback);
			}
			else {
				resetArgs(cmdArg);
				cmdArg.highDataAddress = (current_address >> 8);
				cmdArg.lowDataAddress = (current_address & ~(pageAddressMask));
				cmd = cmdWriteEEPROMMemoryPage;
				processCommand(cmd, cmdArg, readback);
			}

			//Issue command "Poll RDY/_BSY" to check if chip is ready for next command or wait tWD_FLASH before loading next page
			busy = pollEndOperationWithTimeout(twd);
			if ( DEBUG_Internal )
				cout << "Is chip ready to receive a new command? A:" << (busy ? "Busy" : "Ready") << endl;
		}
		else {
			if ( DEBUG_Internal )
				cout << "Skipping address range " << ( current_address & ~(pageAddressMask) ) << "-"
					 << ( current_address | pageAddressMask ) << endl;
		}


		//The program memory must be verified when the program location is skipped and the chip is not erased even
		//if not programmed
		//The data memory verification is skipped when the page is not to be programmed and the chip is
		//erased, since we asssume that the chip is fully erased and that this has been verified.
		//Note: Some chips can have a fuse set that will not erase the data memory on a chip erase,
		//performing a chip erase on those conditions will not result in a fully chip erase.
		if ( verifyWhileWrite && (programPage || !isChipErased) ) {
			//Verify page
			for (current_page_index = (current_address & pageAddressMask);
				 current_page_index < (current_address & pageAddressMask) + page_data_length; current_page_index++) {
				address = (current_address & ~(pageAddressMask)) | current_page_index;
				if ( ( address >= mem->obtem_tamanho() ) || ( address < 0 ) ) {
					throw InvalidAddressException( "Acesso fora do espa�o de endere�amento" );
				}


				mem->le( address, &dataword );
				if ( memoryId.compare("Program") == 0 ) {
					verifyword = readProgramAddress( address );
				}
				else {
					verifyword = readDataAddress( address );
				}

				if ( verifyword != dataword ) {
					//A programming error occurred
					throw ProgrammingErrorException( std::string("Ocorreu um erro de programa��o no endere�o: ") + Utils::int2hexString(address) +
							" expected: " + Utils::int2hexString(dataword) + " read: " + Utils::int2hexString(verifyword));
				}
			}
		}

		//Increment address by written words length
		current_address += page_data_length;
		remaining_data_length -= page_data_length;
	}

	if ( DEBUG_Internal )
		cout << "Finished with remaining data length of: " << remaining_data_length << " bytes" << endl;
}
