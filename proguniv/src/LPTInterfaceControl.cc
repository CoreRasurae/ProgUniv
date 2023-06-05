/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            LPTInterfaceControl.cc
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

#include <memory>
#include <stdio.h>
#include "../include/LPTInterfaceControl.hh"
#include "../include/InvalidLPTPinException.hh"
#include "../include/TimerIF.hh"
#include "../include/UnsupportedPlatformException.hh"

using std::ios;

using namespace std;

#define MAX_MSG_SIZE 255

LPTInterfaceControl *LPTInterfaceControl::instance[] = {0, 0, 0, 0};

LPTInterfaceControl::LPTInterfaceControl()
{
	store_lpt_state = true;
	output_port_cache_is_valid = false;
}

LPTInterfaceControl::~LPTInterfaceControl()
{
}

void LPTInterfaceControl::initLPTInterfaceControl(int address) {
	using namespace InterfaceClass;
	base_lpt = address;

#ifdef __linux__
	lptInterface = LPTLowLevelInterface::createInstance(InterfaceClass::LINUX_PLATFORM_NAME, base_lpt);
#elif __WIN32__
	lptInterface = LPTLowLevelInterface::createInstance(InterfaceClass::WIN_PLATFORM_NAME, base_lpt);
#else
    throw new UnsupportedPlatformException("Unsupported platform");
#endif
}

LPTInterfaceControl& LPTInterfaceControl::getInstance(int address) {
	LPTInterfaceControl *local_instance;
	int index;
	char *errMsg;

	switch (address) {
		case 0x278:
			index = 0;
			break;
		case 0x378:
			index = 1;
			break;
		case 0x3000:
			index = 2;
			break;
		case 0x5cf4:
			index = 3;
			break;
		default:
			//Throw exception (Invalid LPT base address)
			snprintf(errMsg, MAX_MSG_SIZE,
				 	"Invalid LPT Pin ID specified - %0x\n",
				 	address);
			throw InvalidLPTBaseAddressException(errMsg);
	}

	if (LPTInterfaceControl::instance[index] == 0) {
		instance[index] = new LPTInterfaceControl();
		instance[index]->initLPTInterfaceControl(address);
	}
	local_instance = instance[index];

	return *local_instance;
}

void LPTInterfaceControl::setRWMode(bool rwMode) {
	unsigned char byte;

	byte = lptInterface->readByte(base_lpt + 2);
	if (rwMode) {
		byte |= (1 << 5);
		lptInterface->writeByte(base_lpt + 2, byte);
	}
	else {
		byte &= ~(1 << 5);
		lptInterface->writeByte(base_lpt + 2, byte);
	}
}

void LPTInterfaceControl::sendByte(unsigned char byte) {
	cached_output_port_value = byte;
	output_port_cache_is_valid = true;
	lptInterface->writeByte(base_lpt, byte);
}

unsigned char LPTInterfaceControl::readByte() {
	cached_output_port_value = lptInterface->readByte(base_lpt);
	output_port_cache_is_valid = true;
	return cached_output_port_value;
}

void LPTInterfaceControl::setDataBit(int nbit, unsigned char bit_state) {
	//Timer &timer = Timer::getInstance();

	//uint64_t instantA = timer.getCurrentInstant();
	if ( !store_lpt_state || !output_port_cache_is_valid ) {
		cached_output_port_value = lptInterface->readByte(base_lpt);
		output_port_cache_is_valid = true;
	}

	if (bit_state != 0) {
		cached_output_port_value |= (1 << nbit);
	}
	else {
		cached_output_port_value &= ~(1 << nbit);
	}

	lptInterface->writeByte(base_lpt, cached_output_port_value);
	//uint64_t instantB = timer.getCurrentInstant();
	//instantA = instantB - instantA;
	//cout << "Took: " << instantB - instantA << endl;

}

unsigned char LPTInterfaceControl::readDataBit(int nbit) {
	unsigned char valor;

	valor = lptInterface->readByte(base_lpt);
	valor >>= nbit;
	valor &= 0x1;

	return valor;
}

unsigned char LPTInterfaceControl::readnACK() {
	unsigned char valor;

	valor = lptInterface->readByte(base_lpt + 1);
	valor >>= 6;
	valor &= 0x01;

	return valor;
}

unsigned char LPTInterfaceControl::readLPTPin(LPTPin pin) {
	int offset = 0, bit = 0;
	unsigned char valor;

	decodeLPTPin(pin, offset, bit);

	valor = lptInterface->readByte(base_lpt + offset);
	valor >>= bit;
	valor &= 0x01;

	return valor;
}

void LPTInterfaceControl::writeLPTPin(const LPTPin pinId, unsigned char bit_state) {
	int offset = 0, nbit = 0;
	unsigned char valor;

	decodeLPTPin(pinId, offset, nbit);

	if (offset == 0 && store_lpt_state) {
		if ( !output_port_cache_is_valid ) {
			cached_output_port_value = lptInterface->readByte(base_lpt);
			output_port_cache_is_valid = true;
		}

		if (bit_state != 0) {
			cached_output_port_value |= (1 << nbit);
		}
		else {
			cached_output_port_value &= ~(1 << nbit);
		}

		lptInterface->writeByte(base_lpt, cached_output_port_value);
	}
	else {
		valor = lptInterface->readByte(base_lpt + offset);
		if (bit_state != 0) {
			valor = valor | (1 << nbit);
		}
		else {
			valor &= ~(1 << nbit);
		}

		lptInterface->writeByte(base_lpt + offset, valor);
	}
}

__inline__ void LPTInterfaceControl::decodeLPTPin(const LPTPin pinId, int& offset, int& bit) {
	char errMsg[MAX_MSG_SIZE + 1];

	switch (pinId) {
		case LPTInterfaceControl::lptStatusnACK:
			offset=1;
			bit=6;
			break;
		case LPTInterfaceControl::lptStatus_Busy:
			offset=1;
			bit=7;
			break;
		case LPTInterfaceControl::lptStatusPaperOut:
			offset=1;
			bit=5;
			break;
		case LPTInterfaceControl::lptStatusSelect:
			offset=1;
			bit=4;
			break;
		case LPTInterfaceControl::lptStatusnError:
			offset=1;
			bit=3;
			break;
		case LPTInterfaceControl::lptControl_nStrobe:
			offset=2;
			bit=0;
			break;
		case LPTInterfaceControl::lptControl_nAutoLF:
			offset=2;
			bit=1;
			break;
		case LPTInterfaceControl::lptControlnINIT:
			offset=2;
			bit=2;
			break;
		case LPTInterfaceControl::lptControl_nSelect:
			offset=2;
			bit=3;
			break;
		default:
			offset=0;
			bit=0;
			//Unknown LPT pin
			snprintf(errMsg, MAX_MSG_SIZE,
				 	"Invalid LPT Pin ID specified - %d\n",
				 	pinId);
			throw InvalidLPTPinException(errMsg);
	}
}
