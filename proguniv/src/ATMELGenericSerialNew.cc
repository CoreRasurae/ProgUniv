/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            ATMELGenericSerial.cc
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
#include <string>
#include <locale>
#include "../include/ATMELGenericSerial.hh"
#include "../include/Utils.hh"
#include "../include/TimerIF.hh"
#include "../include/RealTime.hh"
#include "../include/InvalidAddressException.hh"
#include "../include/GlobalOptions.hh"

#define MAX_MSG_SIZE 255

using InterfaceClass::TimerIF;
using namespace std;

/**
 * Implements
 */
void ATMELGenericSerial::attachRealProgrammer(shared_ptr<InterfaceClass::ATMELGenericSerialHwIF> prog) {
	programmer = prog;
}

bool ATMELGenericSerial::isExternalClockSourceAvailable() {
    //TODO this information should be cached after first query, since it does not change during runtime
    GlobalOptions &globalOpt = GlobalOptions::getInstance();
    std::string clock = globalOpt.getOption(GlobalOptions::HAS_EXTERNAL_CLOCK_SOURCE);
    if (clock.compare("TRUE") == 0) {
        return true;
    }
    return false;
}

// Implement the implementation class
ATMELGenericSerial::ATMELGenericSerial(shared_ptr<InterfaceClass::ATMELGenericSerialHwIF> realProgrammer, shared_ptr<InterfaceClass::ChipIF> targetChip) {
	this->programmer = realProgrammer;
	this->chip = targetChip;
	DEBUG_Internal = true;
	inProgrammingMode = false;
	fullyDisableClockSignal = false;
	generateClockSignal = true;

	programMemoryTestPattern = false;

	//Extract chip specific timings for serial programming
	Property &clockDelayProp = chip->getProgrammerConfigurationProperty("serialClockDelay");
	Property &cyclesSCK0Prop = chip->getProgrammerConfigurationProperty("serialClockCyclesSCK0");
	Property &cyclesWriteSCK1Prop = chip->getProgrammerConfigurationProperty("serialClockCyclesWriteSCK1");
	Property &cyclesReadSCK1Prop = chip->getProgrammerConfigurationProperty("serialClockCyclesReadSCK1");

	cout << "Chip " << chip->getChipName() << " programming timings:" << endl;
	cout << "  " << clockDelayProp.getDescription() << " --> " << clockDelayProp.getAcceptedValueDetail( clockDelayProp.getValue() ) << endl;
	cout << "  " << cyclesSCK0Prop.getDescription() << " --> " << cyclesSCK0Prop.getAcceptedValueDetail( cyclesSCK0Prop.getValue() )  << endl;
	cout << "  " << cyclesWriteSCK1Prop.getDescription() << " --> " << cyclesWriteSCK1Prop.getAcceptedValueDetail( cyclesWriteSCK1Prop.getValue() )  << endl;
	cout << "  " << cyclesReadSCK1Prop.getDescription() << " --> " << cyclesReadSCK1Prop.getAcceptedValueDetail( cyclesReadSCK1Prop.getValue() )  << endl;


	//These can throw exceptions, but there's nothing we can do to recover
	delay = Utils::string2int( clockDelayProp.getValue() );
	cyclesSCK0 = Utils::string2int( cyclesSCK0Prop.getValue() );
	cyclesWriteSCK1 = Utils::string2int( cyclesWriteSCK1Prop.getValue() );
	cyclesReadSCK1 = Utils::string2int( cyclesReadSCK1Prop.getValue() );
	if ( chip->hasProgrammerConfigurationProperty("requiresExtendedAddress") ) {
		Property &requiresExtAddrProp = chip->getProgrammerConfigurationProperty("requiresExtendedAddress");
		cout << "  " << requiresExtAddrProp.getDescription() << " --> " << requiresExtAddrProp.getAcceptedValueDetail( requiresExtAddrProp.getValue() ) << endl;
		requiresExtendedAddress = Utils::string2bool( requiresExtAddrProp.getValue() );
	}
}

ATMELGenericSerial::~ATMELGenericSerial() {
}

int ATMELGenericSerial::generateCommand(Commands& command, CommandArg& arg, unsigned char* commandBytes) {
	std::string command_string;
	char errMsg[MAX_MSG_SIZE + 1];
	unsigned int i,j;
	int bytes; 				//Number of bytes processed
	char byte_command[5]; 	//String with hexadecimal byte value
	int res; 				//Result code from calling a given function
	char *arg_identifier;   //String that identfies de current command argument to be filled

	switch (command) {
		case cmdProgrammingEnable:
			command_string = "#AC#53#00#00";
			break;

		case cmdChipErase:
			command_string = "#AC#80#00#00";
			break;

		case cmdPollRdy_Bsy:
			command_string = "#F0#00#00%LDBI%";
			break;

		case cmdLoadExtendedAddressByte:
			command_string = "#4D#00%EADR%#00";
			break;

		case cmdWriteProgramMemoryHigh:
			command_string = "#48%ADRH%%ADRL%%HDBO%";
			break;

		case cmdWriteProgramMemoryLow:
			command_string = "#40%ADRH%%ADRL%%LDBO%";
			break;

		case cmdLoadProgramMemoryPageHigh:
			command_string = "#48#00%ADRL%%HDBO%";
			break;

		case cmdLoadProgramMemoryPageLow:
			command_string = "#40#00%ADRL%%LDBO%";
			break;

		case cmdLoadEEPROMMemoryPage:
			command_string = "#C1#00%DADRL%%LDBO%";
			break;

		case cmdReadProgramMemoryHigh:
			command_string = "#28%ADRH%%ADRL%%HDBI%";
			break;

		case cmdReadProgramMemoryLow:
			command_string = "#20%ADRH%%ADRL%%LDBI%";
			break;

		case cmdReadEEPROMMemory:
			command_string = "#A0%DADRH%%DADRL%%LDBI%";
			break;

		case cmdReadLockBits:
			command_string = "#58#00#00%LDBI%";
			break;

		case cmdReadSignatureByte:
			command_string = "#30#00%ADRL%%LDBI%";
			break;

		case cmdReadFuseBits:
			command_string = "#50#00#00%LDBI%";
			break;

		case cmdReadFuseHighBits:
			command_string = "#58#08#00%LDBI%";
			break;

		case cmdReadExtendedFuseBits:
			command_string = "#50#08#00%LDBI%";
			break;

		case cmdReadCalibrationByte:
			command_string = "#38#00#00%LDBI%";
			break;

		case cmdWriteProgramMemoryPage:
			command_string = "#4C%ADRH%%ADRL%#00";
			break;

		case cmdWriteEEPROMMemory:
			command_string = "#C0%DADRH%%DADRL%%LDBO%";
			break;

		case cmdWriteEEPROMMemoryPage:
			command_string = "#C2%DADRH%%DADRL%#00";
			break;

		case cmdWriteLockBits:
			command_string = "#AC#E0#00%LDBO%";
			break;

		case cmdWriteFuseBits:
			command_string = "#AC#A0#00%LDBO%";
			break;

		case cmdWriteFuseHighBits:
			command_string = "#AC#A8#00%LDBO%";
			break;

		case cmdWriteExtendedFuseBits:
			command_string = "#AC#A4#00%LDBO%";
			break;

		default:
			command_string = "";
	};

	if (command_string.length() == 0)
		throw CommandException("Comando ATMEL desconhecido");

	//Processar comando para introduzir argumentos
	bytes = 0;
	for ( i = 0; i < command_string.length(); i++ ) {
		if ( command_string[i] == '#' && ((i+2) < command_string.length()) ) {
			int value;

			//Preencher comando em hexadecimal para convers�o
			byte_command[0] = '0';
			byte_command[1] = 'x';
			byte_command[2] = command_string[i+1];
			byte_command[3] = command_string[i+2];
			byte_command[4] = '\0';

			res = sscanf(byte_command, "%x", &value);
			if (res == EOF || res == 0) {
				snprintf(errMsg, MAX_MSG_SIZE,
				 "Problema ao processar comando %d no byte %d - erro de sintaxe (%s), caracter %d, byte_command (%s)\n",
				 command, bytes, command_string.c_str(), i, byte_command);
				throw CommandException(errMsg);
			}

			//Guardar resultado e incrementar byte do comando ATMEL
			commandBytes[bytes++] = (char)value;
			//Saltar bytes processados
			i+=2;
		}
		else if ( command_string[i] == '%' ) {
			bool bounds = true; //Set to true if array bounds are exceeded
			int substring_size = 0;
			int *value = 0; //Command byte value, before conversion to char value

			//Check string size and array bounds
			i++; //Skip the first % argument delimiter
			for (j = i; j < command_string.length(); j++) {
				if ( (command_string[j] == '%') ) {
					bounds = false;
					break;
				}
				else {
					substring_size++;
				}
			}

			//Validate and extract sub-string
			if (bounds) {
				snprintf(errMsg, MAX_MSG_SIZE,
				 "Problema ao processar comando %d no byte %d - erro de sintaxe (%s), caracter %d - fim de arg. em falta\n",
				 command, bytes, command_string.c_str(), j);
				throw CommandException(errMsg);
			}

			//Copy the argument identifier from the command template to a temporary string array
			arg_identifier = new char[substring_size+1];
			int temp_index = 0;
			for (j = i; j < i+substring_size; j++) {
				arg_identifier[temp_index++] = command_string[j];
			}
			arg_identifier[temp_index] = '\0';
			//Update i index to bypass the argument that was read
			i+=substring_size;

			//Parse, validate and identify argument
			if ( !strcmp(arg_identifier, "ADRL") ) {
				//Low address (all memories except data eeprom)
				value = &arg.lowProgramAddress;
			}
			else if ( !strcmp(arg_identifier, "LDBO") ) {
				//Low data byte output (into chip)
				value = &arg.lowDataByteOut;
			}
			else if ( !strcmp(arg_identifier, "LDBI") ) {
				//Low data byte input (from chip)

				//For now, do nothing
			}
			else if ( !strcmp(arg_identifier, "DADRL") ) {
				//Low data eepprom address
				value = &arg.lowDataAddress;
			}
			else if ( !strcmp(arg_identifier, "ADRH") ) {
				//High address (all memories except data eeprom)
				value = &arg.highProgramAddress;
			}
			else if ( !strcmp(arg_identifier, "HDBO") ) {
				//High data byte output (into chip)
				value = &arg.highDataByteOut;
			}
			else if ( !strcmp(arg_identifier, "HDBI") ) {
				//High data byte input (from chip)

				//For now, do nothing
			}
			else if ( !strcmp(arg_identifier, "DADRH") ) {
				//High data eepprom address
				value = &arg.highDataAddress;
			}
			else if ( !strcmp(arg_identifier, "EADR") ) {
				//Extended address
				value = &arg.extendedAddress;
			}
			else {
				//Unknown command argument
				snprintf(errMsg, MAX_MSG_SIZE,
				 	"Argumento desconhecido no comando %d no byte %d - erro de sintaxe (%s), argumento %s\n",
					 	command, bytes, command_string.c_str(), arg_identifier);
					throw CommandException(errMsg);
			}

			if (value != 0) {
				//This is an output argument
				if (*value == -1) {
					//Trying to use an unspecified argument value
					snprintf(errMsg, MAX_MSG_SIZE,
				 		"Argumento para o comando %d no byte %d com valor vazio (argumento %s)\n",
				 		command, bytes, arg_identifier);
					throw CommandException(errMsg);
				}

				commandBytes[bytes++] = (char)*value;
			}

			delete[] arg_identifier;
		}
		else {
			snprintf(errMsg, MAX_MSG_SIZE,
				 "Problema ao processar comando %d no byte %d - erro de sintaxe (%s), caracter %d\n",
				 command, bytes, command_string.c_str(), i);
			throw CommandException(errMsg);
		}
	}

	return bytes;
}

int ATMELGenericSerial::serialSendByte(unsigned char sendByte, unsigned char *receivedByte) {
	using InterfaceClass::ATMELGenericSerialHwIF;
	register uint8_t delay = this->delay;
	register uint8_t cyclesSCK0 = this->cyclesSCK0;
	register uint8_t cyclesSCK1 = this->cyclesWriteSCK1;


	int i;
	int nbit = 0;

	unsigned char bit;
	//printf("Sending: %02x\n", sendByte);

	TimerIF& timer = TimerIF::getInstance();

        if (receivedByte != 0) {
        	*receivedByte = 0;
        }

	for (nbit = 0; nbit < 8; nbit++) {
		programmer->setSCK(0);
		timer.delay(2);
		if ( generateClockSignal ) {
			for (i = 0; i < cyclesSCK0; i++) {
				if (i == 0) {
					//Write the bit
					bit = (sendByte & 0x80) != 0 ? 1 : 0;
					programmer->setMOSI(bit);
					sendByte <<= 1;
				}

				programmer->setCLK(0);
				timer.delay(delay);

				programmer->setCLK(1);
				timer.delay(delay);
			}
		} else {
			//Write the bit
			bit = (sendByte & 0x80) >> 7;
			programmer->setMOSI(bit);
			sendByte <<= 1;

			timer.delay(1);
		}

		programmer->setSCK(1);
		if ( generateClockSignal ) {
			for (i = 0; i < cyclesSCK1; i++) {
				if (i == 0 && receivedByte != 0) {
					//Read the bit
					bit = programmer->getMISO();
					*receivedByte <<= 1;
					if (bit == 1)
						*receivedByte |= 0x01;
				}

				programmer->setCLK(0);
				timer.delay(delay);
				programmer->setCLK(1);
				timer.delay(delay);				
			}
		}
		else {
			//Since the chip or programmer generates it's own clock signal, we only need to send the data
			timer.delay(1);
			if (receivedByte != 0) {
				//Read the bit
				bit = programmer->getMISO();
				*receivedByte <<= 1;
				if (bit == 1)
					*receivedByte |= 0x01;
			}

		}		
	}
	
	programmer->setSCK(0);
	
	return 0;
}

int ATMELGenericSerial::serialReceiveByte(unsigned char *byte) {
	using InterfaceClass::ATMELGenericSerialHwIF;
	register uint8_t delay = this->delay;
	register uint8_t cyclesSCK0 = this->cyclesSCK0;
	register uint8_t cyclesSCK1 = this->cyclesReadSCK1;

	unsigned char bit;

	int nbit;
	int i;

	TimerIF& timer = TimerIF::getInstance();

	//programmer->setSCK(0);
	//timer.delay(2);
	for (nbit = 0; nbit < 8; nbit++) {
		programmer->setSCK(0);
		if ( generateClockSignal ) {
			for (i = 0; i < cyclesSCK0; i++) {
				programmer->setCLK(0);
				timer.delay(delay);
				programmer->setCLK(1);
				timer.delay(delay);
			}
		}
		else {
			//Since the chip or programmer generates it's own clock signal, we only need to send the data
			timer.delay(1);

			//Read the bit
			bit = programmer->getMISO();
			*byte <<= 1;
			if (bit == 1)
				*byte |= 0x01;
		}

		if (nbit < 8) {
			programmer->setSCK(1);

			if ( generateClockSignal ) {
				for (i = 0; i < cyclesSCK1; i++) {
					programmer->setCLK(0);
					timer.delay(delay);
					programmer->setCLK(1);
					timer.delay(delay);
					
					if ( i == (cyclesSCK1 -1) ) {
						//Read the bit
						bit = programmer->getMISO();
						*byte <<= 1;
						if (bit == 1) {
							*byte |= 0x01;
						}
					}

				}
			}
			else {
				//Since the chip or programmer generates it's own clock signal, we only need to send the data
				timer.delay(1);
			}
		}

		//programmer->setSCK(0);
	}

	programmer->setSCK(0);
	
	return 0;
}


int ATMELGenericSerial::processCommand(Commands& command, CommandArg& arg, unsigned char received[4], bool forceClockSignal) {
	unsigned char byteCommand[4];
	int i;
	int bytes_to_send;
	int received_bytes = 0;

	//Only disable clock signal generation if the chip has an internal clock and we
	//are sure that it is the current chip clock source, since at startup we don't know the
	//chip configuration and we must get the chip into the programming mode.
	//One situation where this measure isn't required is when the programmer hardware itself has an
	//external clock source and it is active.
	Property &internalClock = chip->getProgrammerConfigurationProperty("InternalClockSourceActive");
	if ( ( !Utils::string2bool( internalClock.getValue() ) && isExternalClockSourceAvailable()) || fullyDisableClockSignal ||
	         ( Utils::string2bool( internalClock.getValue() ) && !forceClockSignal && chip->getInternalClockFrequency() >= 1000000) ) {
		cout << "Using available hardware clock, instead of generating the clock signal" << endl;
		generateClockSignal = false;
	}
	else {
		cout << "Generating clock";
		if (fullyDisableClockSignal) {
			cout << " because fully disable clock signal is active" << endl;
		} else if (forceClockSignal) {
			cout << " because force clock signal is active" << endl;
		} else if (!Utils::string2bool( internalClock.getValue())) {
			cout << " because internal clock is false - actual value: " << internalClock.getValue() << endl;
		} else if (chip->getInternalClockFrequency() < 1000000) {
			cout << " because no clock with frequency above or equal to 1Mhz is available" << endl;
		} else {
			cout << " because of Unknown cause" << endl;
		}
		generateClockSignal = true;
	}

	bytes_to_send = generateCommand(command, arg, byteCommand);

	for (i = 0;  i < bytes_to_send; i++) {
		if (received != 0)
			serialSendByte(byteCommand[i], &received[i]);
		else
			serialSendByte(byteCommand[i], 0);
	}
	if (i == 3) {
		//Commands are 4bytes long, we've only sent 3, so this is the 4th byte and is input only
		if ( received != 0 ) {
			serialReceiveByte(&received[i]);
			received_bytes = 1;
		}
	}

	return received_bytes;
}

void ATMELGenericSerial::serialPowerUp() {
	using InterfaceClass::ATMELGenericSerialHwIF;
	int i;

	TimerIF& timer = TimerIF::getInstance();

	programmer->setPOWER(ATMELGenericSerialHwIF::CHIP_POWER_OFF);
	timer.delay(500);
	programmer->setCLK(0);
	programmer->setSCK(0);
	programmer->setRESET(ATMELGenericSerialHwIF::CHIP_RESET);
	timer.delay(500);
	programmer->setPOWER(ATMELGenericSerialHwIF::CHIP_POWER_ON);

	//Sometimes the above initialization procedure does not work.
	//According to the ATMEL specs it is recommended in these cases
	// to give a normal operation pulse, just after placing the SCK pin
	// at a low level while providing a clock BURST and then entering
	// RESET state again. The ATMEL AT90S2313 only worked after this
	// recovery initialization.
	programmer->setCLK(0);
	programmer->setSCK(0);
	programmer->setRESET(ATMELGenericSerialHwIF::CHIP_OPERATION);
	timer.delay(128000);

	if ( !fullyDisableClockSignal ) {
		for (i = 0; i < 32; i++) {
			programmer->setCLK(0);
			timer.delay(3); //1us delay
			programmer->setCLK(1);
			timer.delay(3);
		}
	}
	else {
		timer.delay(1);
	}

	programmer->setRESET(ATMELGenericSerialHwIF::CHIP_RESET);
	programmer->setPOWER(ATMELGenericSerialHwIF::CHIP_POWER_ON);
}

void ATMELGenericSerial::serialPowerOff() {
	using InterfaceClass::ATMELGenericSerialHwIF;

	inProgrammingMode = false;
	programmer->setCLK(0);
	programmer->setSCK(0);
	programmer->setRESET(ATMELGenericSerialHwIF::CHIP_OPERATION);
	programmer->setPOWER(ATMELGenericSerialHwIF::CHIP_POWER_OFF);
}

bool ATMELGenericSerial::serialEnterProgramming(bool forceClockSignal) {
	unsigned char readback[4];
	bool initOkFlag = false;
	Commands command;
	CommandArg arg;
	TimerIF& timer = TimerIF::getInstance();

	if (chip->getExternalClockFrequency() >= 1000000) {
		cout << "Disabling clock generation due to external clock availability with frequency: "
		     <<  chip->getExternalClockFrequency() << endl;
		fullyDisableClockSignal = true;
	} else {
		fullyDisableClockSignal = false;
	}

	timer.delay(21000); //Delay 21ms before sending the programmingEnable command

	command = cmdProgrammingEnable;
	resetArgs(arg);

	processCommand(command, arg, readback, forceClockSignal);
	if (DEBUG_Internal)
		printf("Leitura do Programming Enable: %02x %02x %02x %02x\n", readback[0], readback[1], readback[2], readback[3] );

	if (readback[2] == 0x53 && readback[3] == 0x00) {
		//Update the inProgrammingMode flag state
		inProgrammingMode = true;
		initOkFlag = true;
	}

	return initOkFlag;
}

void ATMELGenericSerial::serialPowerUpEnterProgramming(bool forceClockSignal) {
	int i;

	if (chip->getExternalClockFrequency() >= 1000000) {
		cout << "Disabling clock generation due to external clock presence" << endl;
		fullyDisableClockSignal = true;
	} else {
		fullyDisableClockSignal = false;
	}

	//The ATMEL Programming Specification suggests that one should make 32
	// attemps before considering no Device Present.
	for (i = 0; i < 32; i++) {
		serialPowerUp();
		if ( serialEnterProgramming( forceClockSignal ) )
			break;
	}
	if (i == 32) {
		serialPowerOff();
		throw NotInProgrammingModeException("Inicialização do chip falhou, não foi possível entrar no modo de programação");
	}
}

bool ATMELGenericSerial::pollEndOperationWithTimeout(const unsigned int timeout) {
	bool busy = true;
	bool do_poll = true;
	uint64_t accum = 0;
	uint64_t instantA, instantB, last_delta = 0;

	Commands cmd;
	CommandArg cmdArg;
	unsigned char readback[4];
	TimerIF& timer = TimerIF::getInstance();

	int count  = 0;
	readback[3] = 1;
	instantA = timer.getCurrentInstant();
	while ( busy && (accum <= timeout) ) {
		if (do_poll) {
			resetArgs(cmdArg);
			cmd = cmdPollRdy_Bsy;
			processCommand(cmd, cmdArg, readback);
			count++;
		}

		instantB = timer.getCurrentInstant();
		last_delta = instantB - instantA;
		accum += last_delta;

		if ( (readback[3] & 0x01) == 0 && accum < timeout) {
			busy = false;
		}

		//Check if we should perform another poll, or just busy wait
		if (last_delta + accum > timeout)
			do_poll = false;

		//Let's update the start instant
		instantA = instantB;
	}


	//Only perform busy verification, if the chip was busy (timeout occurred)
	//and the poll command can be issued under 1 msec
	if (busy && last_delta <= 1000) {
		resetArgs(cmdArg);
		cmd = cmdPollRdy_Bsy;
		processCommand(cmd, cmdArg, readback);
		if ( (readback[3] & 0x01) == 0 )
			busy = false;
		count++;
	}

	cout << "Accum: " << accum << " Timeout: " << timeout << " Count: " << count << endl;

	return busy;
}

void ATMELGenericSerial:: updateExtendedAddressIfRequired(unsigned int newAddress) {
	Commands cmd;
	CommandArg cmdArg;
	unsigned char readback[4];
	
	if (requiresExtendedAddress && currentExtendedAddress != (newAddress >> 16)) {
	   resetArgs(cmdArg);
	   cmdArg.extendedAddress = (newAddress >> 16);
	   currentExtendedAddress = cmdArg.extendedAddress;
	   cmd = cmdLoadExtendedAddressByte;
	   processCommand(cmd, cmdArg, readback);				   
	}
}

unsigned int ATMELGenericSerial::readProgramAddress(unsigned int address) {
	Commands cmd;
	CommandArg cmdArg;
	unsigned int dataword;
	unsigned char readback[4];

	if ( !inProgrammingMode )
		throw NotInProgrammingModeException("O chip não se encontra no modo de programação");

	updateExtendedAddressIfRequired(address);

	resetArgs(cmdArg);
	cmdArg.highProgramAddress = (address >> 8) & 0xff;
	cmdArg.lowProgramAddress = address & 0xff;
	cmd = cmdReadProgramMemoryLow;
	processCommand(cmd, cmdArg, readback);

	dataword = readback[3];

	cmd = cmdReadProgramMemoryHigh;
	processCommand(cmd, cmdArg, readback);

	dataword |= (readback[3] << 8);

	return dataword;
}

unsigned char ATMELGenericSerial::readDataAddress(unsigned int address) {
	Commands cmd;
	CommandArg cmdArg;
	unsigned char databyte;
	unsigned char readback[4];

	if ( !inProgrammingMode )
		throw NotInProgrammingModeException("O chip não se encontra no modo de programação");

	resetArgs(cmdArg);
	cmdArg.highDataAddress = (address >> 8) & 0xff;
	cmdArg.lowDataAddress = address & 0xff;
	cmd = cmdReadEEPROMMemory;
	processCommand(cmd, cmdArg, readback);

	databyte = readback[3];

	return databyte;
}


void ATMELGenericSerial::writeMemoryInternal(const bool verifyWhileWrite, const bool isChipErased, const std::string &memoryId, const bool useTestData) {
	Commands cmd;
	CommandArg cmdArg;
	unsigned int dataword;
	unsigned char readback[4];
	unsigned int  address;

	if ( useTestData )
		generateTestData( memoryId );

	Memoria *mem = 0;
	unsigned int twd;

	if ( memoryId.compare("Program") == 0 ) {
		mem = chip->getMemory(memoryId);
		if ( mem == 0 )
			throw ProgrammingErrorException("Memory name: " + memoryId + " not found, please check this chip implementation");

		//get twd Times
		try {
			Property &prop = chip->getProgrammerConfigurationProperty("minVddTwd_flash");
			twd = Utils::string2int( prop.getValue() );
		} catch ( exception &e ) {
			throw ProgrammingErrorException( "Unable to obtain Chip property: minVddTwd_flash, cause: " + std::string(e.what()) );
		}
	}
	else if ( memoryId.compare("Data") == 0 ) {
		mem = chip->getMemory(memoryId);
		if ( mem == 0 )
			throw ProgrammingErrorException("Memory name: " + memoryId + " not found, please check this chip implementation");

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
		throw ProgrammingErrorException("Don't know how to handle memory: " + memoryId);
	}

	if ( !inProgrammingMode )
		throw NotInProgrammingModeException("O chip não se encontra no modo de programação");

	bool busy;
	uint64_t instantA, instantB, accum, timeout;
	//Flag that indicates if a data memory location should be programmed or not
	bool programLocation;
	int retries = 0;

	for (address = 0;  address < mem->obtem_tamanho(); address++ ) {
		//Read data to Program
		mem->le(address, &dataword);

		resetArgs(cmdArg);
		if ( memoryId.compare("Program") == 0 ) {
			if ( !isChipErased || ((dataword >> 8) & 0xff) != 0xff ) {
				//Only program this position if high byte is different than 0xff (empty)
				//0xFF values can never be programmed, either they are already 0xFF or else a programming error would occur anyway.
				//It is sufficient to ensure that the page on the chip matches the page to be programmed (later in verification step).
				if ( ((dataword >> 8) & 0xff) != 0xff ) {

					cmdArg.highProgramAddress = (address >> 8) & 0xff;
					cmdArg.lowProgramAddress = address & 0xff;
					cmdArg.highDataByteOut = (dataword >> 8) & 0xff;
					cmd = cmdWriteProgramMemoryHigh;
					processCommand(cmd, cmdArg, readback);

					busy = true;
					accum = 0;
					timeout = twd;
					while ( busy && (accum <= timeout) ) {
						instantA = TimerIF::getInstance().getCurrentInstant();
						resetArgs(cmdArg);
						cmdArg.highProgramAddress = (address >> 8) & 0xff;
						cmdArg.lowProgramAddress = address & 0xff;
						cmd = cmdReadProgramMemoryHigh;
						processCommand(cmd, cmdArg, readback);

						if ( (((dataword >> 8) & 0xff) != 0x7f) ) {
							if ( readback[3] == 0x7f )
								busy = true;
							else if ( readback[3] == ((dataword >> 8) & 0xff) )
								busy = false;
						}

						instantB = TimerIF::getInstance().getCurrentInstant();
						accum += instantB - instantA;
					}
				}

				//Since we assume that all memory locations are 0xff when the
				//chip is erased and that this has been previously verified,
				//we can skip verification in that case
				if ( verifyWhileWrite ) {
					resetArgs(cmdArg);
					cmdArg.highProgramAddress = (address >> 8) & 0xff;
					cmdArg.lowProgramAddress = address & 0xff;
					cmd = cmdReadProgramMemoryHigh;
					processCommand(cmd, cmdArg, readback);
					if (readback[3] != ((dataword >> 8) & 0xff) ) {
						if (retries < 3) {
							address--;
							retries++;
							continue;
						}

						throw ProgrammingErrorException("Error while programming word (High Byte) at address: " + Utils::int2hexString(address) +
								" byte: " + Utils::int2hexString((dataword >> 8)& 0xff));
					}
					else
						retries = 0;
				}
			}

			if ( !isChipErased || (dataword & 0xff) != 0xff ) {
				//Only program this position if low byte is different than 0xff (empty)
				//0xFF values can never be programmed, either they are already 0xFF or else a programming error would occur anyway.
				//It is sufficient to ensure that the page on the chip matches the page to be programmed (later in verification step).
				if ( (dataword & 0xff) != 0xff ) {
					resetArgs(cmdArg);
					cmdArg.highProgramAddress = (address >> 8) & 0xff;
					cmdArg.lowProgramAddress = address & 0xff;
					cmdArg.lowDataByteOut = dataword & 0xff;
					cmd = cmdWriteProgramMemoryLow;
					processCommand(cmd, cmdArg, readback);

					busy = true;
					accum = 0;
					timeout = twd;
					while ( busy && (accum <= timeout) ) {
						instantA = TimerIF::getInstance().getCurrentInstant();
						resetArgs(cmdArg);
						cmdArg.highProgramAddress = (address >> 8) & 0xff;
						cmdArg.lowProgramAddress = address & 0xff;
						cmd = cmdReadProgramMemoryLow;
						processCommand(cmd, cmdArg, readback);

						if ( ((dataword & 0xFF) != 0x7F) ) {
							if ( readback[3] == (address & 0xff) )
								busy = true;
							else if ( readback[3] == (dataword & 0xFF) )
								busy = false;
						}

						instantB = TimerIF::getInstance().getCurrentInstant();
						accum += instantB - instantA;
					}
				}
			}

			//Since we assume that all memory locations are 0xff when the
			//chip is erased and that this has been previously verified,
			//we can skip verification in that case
			if ( verifyWhileWrite ) {
				resetArgs(cmdArg);
				cmdArg.highProgramAddress = (address >> 8) & 0xff;
				cmdArg.lowProgramAddress = address & 0xff;
				cmd = cmdReadProgramMemoryLow;
				processCommand(cmd, cmdArg, readback);
				if (readback[3] != (dataword & 0xff) ) {
					if (retries < 3) {
						address--;
						retries++;
						continue;
					}

					throw ProgrammingErrorException("Error while programming word (Low Byte) at address: " + Utils::int2hexString(address) +
							" byte: " + Utils::int2hexString(dataword & 0xff));
				}
				else
					retries = 0;
			}
		}
		else {
			if ( !isChipErased || (dataword & 0xff) != 0xff ) {
				programLocation = false;
				//EEPROM memory is automatically erased, in opposition to the Program Memory that must be erased explicitly
				//So we have to check the individual values to see if there is a need to program that address.
				if (!isChipErased) {
					//We only want to program the memory location when the chip
					//is not erased, if the old value is different than the new
					//one
					resetArgs(cmdArg);
					cmdArg.highDataAddress = (address >> 8) & 0xff;
					cmdArg.lowDataAddress = address & 0xff;
					cmd = cmdReadEEPROMMemory;
					processCommand(cmd, cmdArg, readback);
					if (readback[3] != (dataword & 0xff) ) {
						programLocation = true;
					}
				}
				else {
					//We know that the data memory is erased so we have to
					//program the current location
					programLocation = true;
				}

				if ( programLocation ) {
					cmdArg.highDataAddress = (address >> 8) & 0xff;
					cmdArg.lowDataAddress = address & 0xff;
					cmdArg.lowDataByteOut = dataword & 0xff;
					cmd = cmdWriteEEPROMMemory;
					processCommand(cmd, cmdArg, readback);

					busy = true;
					accum = 0;
					timeout = twd;
					while ( busy && (accum <= timeout) ) {
						instantA = TimerIF::getInstance().getCurrentInstant();
						resetArgs(cmdArg);
						cmdArg.highDataAddress = (address >> 8) & 0xff;
						cmdArg.lowDataAddress = address & 0xff;
						cmd = cmdReadEEPROMMemory;
						processCommand(cmd, cmdArg, readback);

						if ( ((dataword & 0xFF) != 0x7F) && ((dataword & 0xFF) != 0x80)) {
							if ( readback[3] == 0x7F || readback[3] == 0x80 )
								busy = true;
							else
								busy = false;
						}

						instantB = TimerIF::getInstance().getCurrentInstant();
						accum += instantB - instantA;
					}

					if (busy && (dataword & 0xff) != 0x7f)
						cout << "Busy on Data " << (int)(readback[3]) << endl;

					//Since we assume that all memory locations are 0xff when the
					//chip is erased and that this has been previously verified,
					//we can skip verification in that case
					if ( verifyWhileWrite ) {
						resetArgs(cmdArg);
						cmdArg.highDataAddress = (address >> 8) & 0xff;
						cmdArg.lowDataAddress = address & 0xff;
						cmd = cmdReadEEPROMMemory;
						processCommand(cmd, cmdArg, readback);
						if (readback[3] != (dataword & 0xff) ) {
							throw ProgrammingErrorException("Error while programming EEPROM byte at address: " + Utils::int2hexString(address) );
						}
					}
				}
			}
		}
	}
}

void ATMELGenericSerial::generateTestData(const std::string &memoryName) {
	//Temps
	std::string temp_data = "Luís Mendes... teste 1234567890";
	unsigned int i, aux;

	Memoria *mem = chip->getMemory(memoryName);
	if (mem == 0) {
		throw MemoryNotFoundException( "Memoria não encontrada" );
	}

	aux = 0;
	for (i = 0; i < mem->obtem_tamanho(); i++) {
		if ( memoryName.compare( "Program" ) == 0 ) {
			mem->escreve( i, (unsigned int)( (((unsigned char)temp_data[aux + 1]) << 8) | ((unsigned char)temp_data[aux]) ) );
			aux += 2;
		}
		else {
			mem->escreve( i, ((unsigned char)temp_data[aux++]) );
		}

		if ( aux >= temp_data.length() )
			aux = 0;
	}
}

void ATMELGenericSerial::setParameter(const std::string &parameter, const std::string &value) {

	//TODO Build utils method to the case insensitive compare between 2 strings
	//Set debug parameter control and clock generation control
	if ( parameter.compare("DEBUG_Internal") == 0 ) {
		try {
			if ( Utils::string2bool(value) ) {
				DEBUG_Internal = true;
			}
			else {
				DEBUG_Internal = false;
			}
		}
		catch ( ConversionException &ce ) {
			throw InvalidParameterValueException( "Valor para parâmetro " + parameter +
					" é inválido, valores aceites (true ou false), valor: " + value );
		}
	}
	else if ( parameter.compare("FullyDisableClockSignal") == 0 ) {
		try {
			if ( Utils::string2bool(value) ) {
				fullyDisableClockSignal = true;
			}
			else {
				fullyDisableClockSignal = false;
			}
		}
		catch ( ConversionException &ce ) {
			throw InvalidParameterValueException( "Valor para parâmetro " + parameter +
					" é inválido, valores aceites (true ou false), valor: " + value );
		}
	}
	else if ( parameter.compare("GenerateTestPattern") == 0 ) {
		try {
			if ( Utils::string2bool(value) ) {
				programMemoryTestPattern = true;
			}
			else {
				programMemoryTestPattern = false;
			}
		}
		catch ( ConversionException &ce ) {
			throw InvalidParameterValueException( "Valor para parâmetro " + parameter +
					" é inválido, valores aceites (true ou false), valor: " + value );
		}
	}
	else {
		throw UnknownParameterNameException( "Parâmetro " + parameter + " desconhecido" );
	}
}

//Programmer Interface Methods
bool ATMELGenericSerial::checkChipSignature() {
	unsigned char readback[4];
	unsigned char signature[3];
	char signatureString[7];
	int i;
	Commands cmd;
	CommandArg cmdArg;

	serialPowerUpEnterProgramming(true);

	cmd = cmdReadSignatureByte;
	for (i = 0; i < 3; i++) {
		resetArgs(cmdArg);
		cmdArg.lowProgramAddress = i;
		processCommand(cmd, cmdArg, readback, true);
		signature[i] = readback[3];
	}

	serialPowerOff();

	if (DEBUG_Internal)
		printf("Assinatura do chip: %02x %02x %02x\n", signature[0], signature[1], signature[2]);

	snprintf(signatureString, 7, "%02X%02X%02X", signature[0], signature[1], signature[2]);

	bool result = chip->checkSignature(signatureString);

	if (DEBUG_Internal) {
		if (result)
			printf("Chip Signature Matches\n");
		else
			printf("Chip Signature Fails\n");
	}

	return result;
}

void ATMELGenericSerial::writeMemory(const std::string &memoryId, bool verifyWhileWrite, bool isChipErased) {
	serialPowerUpEnterProgramming();

	//"ALL" memories means only Program and Data(EEPROM) memories, excluding all fuses
	if ( memoryId.compare("ALL") == 0 ) {
		std::string memoriesIdsArray[] = { "Program", "Data" };
		unsigned int memoriesIdsSize = sizeof(memoriesIdsArray) / sizeof(std::string);
		unsigned int i;

		for (i = 0; i < memoriesIdsSize; i++)
			writeMemoryInternal( verifyWhileWrite, isChipErased, memoriesIdsArray[i], programMemoryTestPattern );
	}
	else {
		//A single memory is to be programmed
		writeMemoryInternal( verifyWhileWrite, isChipErased, memoryId, programMemoryTestPattern );
	}

	serialPowerOff();
}

void ATMELGenericSerial::readMemory(const std::string &memoryId) {

	unsigned int i;
	unsigned int dataword;

	//"ALL" memories means only Program and Data(EEPROM) memories, excluding all fuses
	if ( memoryId.compare("ALL") == 0 ) {
		std::string memoriesIdsArray[] = { "Program", "Data" };
		unsigned int memoriesIdsSize = sizeof(memoriesIdsArray) / sizeof(std::string);

		for (i = 0; i < memoriesIdsSize; i++)
			readMemory( memoriesIdsArray[i] );

		return;
	}

	if ( memoryId.compare("Program") != 0 && memoryId.compare("Data") != 0 )
		throw ReadingErrorException("Invalid Memory Selected: " + memoryId);

	//Prepare to read
	Memoria *mem = chip->getMemory(memoryId);
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}

	serialPowerUpEnterProgramming();

	//Ler todas as palavras da mem�ria de programa
	mem->fill_value( 0xffff, 0x0000, mem->obtem_tamanho() );
	for (i = 0; i < mem->obtem_tamanho(); i++) {
		if ( memoryId.compare("Program") == 0 )
			dataword = readProgramAddress(i);
		else
			dataword = readDataAddress(i);

		mem->escreve(i, dataword);
	}

	serialPowerOff();
}

void ATMELGenericSerial::eraseMemory(const std::string &memoryId) {
	using InterfaceClass::ATMELGenericSerialHwIF;
	unsigned char leitura;
	RealTime& realTime = RealTime::getInstance();
	TimerIF& timer = TimerIF::getInstance();

	realTime.leaveRTMode();
	char c;
	cout << "setting power on" << endl;
	cin.get(c);
	realTime.enterRTMode();

	programmer->setPOWER(ATMELGenericSerialHwIF::CHIP_POWER_ON);

	realTime.leaveRTMode();
	cout << "activating reset" << endl;
	cin.get(c);
	realTime.enterRTMode();

	programmer->setRESET(ATMELGenericSerialHwIF::CHIP_RESET);

	realTime.leaveRTMode();
	cout << "deactivating reset" << endl;
	cin.get(c);
	realTime.enterRTMode();

	programmer->setRESET(ATMELGenericSerialHwIF::CHIP_OPERATION);
	timer.delay(128000);


	realTime.leaveRTMode();
	cout << "setting SCK clock high" << endl;
	cin.get(c);
	realTime.enterRTMode();

	programmer->setSCK(1);

	realTime.leaveRTMode();
	cout << "setting MOSI high" << endl;
	cin.get(c);
	realTime.enterRTMode();

	programmer->setMOSI(1);

	realTime.leaveRTMode();
	cout << "setting CLK high" << endl;
	cin.get(c);
	realTime.enterRTMode();

	programmer->setCLK(1);

	realTime.leaveRTMode();
	cout << "reading MISO" << endl;
	cin.get(c);
	realTime.enterRTMode();

	leitura = programmer->getMISO();

	realTime.leaveRTMode();
	cout << "leitura " << ((int)(leitura)) << " - reading MISO" << endl;
	cin.get(c);
	realTime.enterRTMode();

	leitura = programmer->getMISO();

	realTime.leaveRTMode();
	cout << "leitura " << ((int)(leitura)) << " - setting power OFF" << endl;
	cin.get(c);
	realTime.enterRTMode();

	programmer->setPOWER(ATMELGenericSerialHwIF::CHIP_POWER_OFF);
}

void ATMELGenericSerial::writeConfiguration() {
	using InterfaceClass::ATMELGenericSerialHwIF;
	TimerIF& timer = TimerIF::getInstance();
	RealTime& realTime = RealTime::getInstance();
	bool realTimeModeOn = false;
	unsigned int valor;
	bool busy = false;

	Commands cmd;
	CommandArg cmdArg;
	unsigned char readback[4];


	//TODO Add support for non-paged chips lock configuration fuse serial command

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

	serialPowerUpEnterProgramming( true );

	Memoria *mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}

	resetArgs( cmdArg );
	mem->le( &valor );
	cmdArg.lowDataByteOut = valor;
	cmd = cmdWriteFuseHighBits;
	processCommand(cmd, cmdArg, readback, true);

	//Issue command "Poll RDY/_BSY" to check if chip is ready for next command or wait tWD_FLASH before loading next paged
	//busy = pollEndOperationWithTimeout(twd);
	timer.nonActiveLowPrecisionDelay(twd);

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
	processCommand(cmd, cmdArg, readback, true);

	//Issue command "Poll RDY/_BSY" to check if chip is ready for next command
	//or wait tWD_FLASH before loading next paged
	timer.nonActiveLowPrecisionDelay(twd);

	cout << "Chip dispon�vel para novo comando: " << busy << endl;

	serialPowerOff();

	if (!realTimeModeOn)
		realTime.leaveRTMode();
}

void ATMELGenericSerial::readConfiguration() {
	using InterfaceClass::ATMELGenericSerialHwIF;
	RealTime& realTime = RealTime::getInstance();
	bool realTimeModeOn = false;

	Commands cmd;
	CommandArg cmdArg;
	unsigned char readback[4];

	realTimeModeOn = realTime.getRTMode();
	realTime.enterRTMode();

	//TODO Non-paged AVR chips do not support the reading of the configuration fuses in serial mode

	//Read all configuration memories
	//Prepare to read lock bit byte
	Memoria *mem = chip->getMemory("Lock Bit Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}

	serialPowerUpEnterProgramming( true );

	resetArgs(cmdArg);
	cmd = cmdReadLockBits;
	processCommand(cmd, cmdArg, readback, true);
	mem->escreve( readback[3] );


	//Prepare to read fuse high byte
	mem = chip->getMemory("Fuse High Byte");
	if (mem == 0) {
		printf("Memoria não encontrada\n");
		return;
	}

	resetArgs(cmdArg);
	cmd = cmdReadFuseHighBits;
	processCommand(cmd, cmdArg, readback, true);
	mem->escreve( readback[3] );

	serialPowerOff();

	if (!realTimeModeOn)
		realTime.leaveRTMode();
}

void ATMELGenericSerial::eraseConfiguration() {
}

void ATMELGenericSerial::eraseAll() {
	TimerIF& timer = TimerIF::getInstance();
	Commands cmd;
	CommandArg cmdArg;
	unsigned char readback[4];
	bool busy;

	serialPowerUpEnterProgramming( true );

	resetArgs(cmdArg);
	cmd = cmdChipErase;
	processCommand(cmd, cmdArg, readback, true);

	unsigned int twd = 11000;
	try {
		Property &prop = chip->getProgrammerConfigurationProperty("minVddTwd_erase");
		twd = Utils::string2int( prop.getValue() );
	} catch (exception &e) {
		throw EraseErrorException( "Unable to obtain minVddTwd_erase time: " + std::string(e.what()) );
	}

	bool pagedMem = true;
	try {
		Property &prop = chip->getProgrammerConfigurationProperty("MemoryType");
		if ( prop.getValue().compare("NonPaged") == 0 )
			pagedMem = false;
	} catch ( InvalidPropertyNameException &e ) {
		throw EraseErrorException( "Unable to obtain chip flash memory type from chip properties: " + std::string(e.what()) );
	}


	if ( pagedMem ) {
		if (DEBUG_Internal)
			cout << "Performing polled erase with timeout on paged chip" << endl;

		//Issue command "Poll RDY/_BSY" to check if chip is ready for next command or wait tWD_FLASH before loading next paged
		busy = pollEndOperationWithTimeout(twd);
		cout << "Chip dispon�vel para novo comando: " << busy << endl;

		serialPowerOff();
		timer.delay(500);
		serialPowerUpEnterProgramming( true );
	}
	else {
		if (DEBUG_Internal)
			cout << "Performing blind erase delay on non paged chip" << endl;

		timer.nonActiveLowPrecisionDelay(twd);

		//Required: For example AT90S2313 will only program correctly Data or Flash memories, if
		//the chip is powered off with a small delay before entering and leaving the programming state
		//before performing the programming, or
		//if the Data or Flash first memory posotion to be programmed is written twice,
		//the first write will fail for both high and low bytes and succeed at the second
		//write, after that all writes will occur as expected
		serialPowerOff();
		timer.delay(500);
		serialPowerUpEnterProgramming( true );
	}

	serialPowerOff();
	programmer->setRESET(InterfaceClass::ATMELGenericSerialHwIF::CHIP_RESET);

}

InterfaceClass::ChipIF *ATMELGenericSerial::getChip() {
	return chip.get();
}

int ATMELGenericSerial::getOperationResult() {
	return 0;
}
