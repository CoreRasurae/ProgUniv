#ifdef __WIN32__
#include <inpout32.h>
#endif
#include <stdio.h>
#include "../include/LPTWinLowLevel.hh"

unique_ptr<InterfaceClass::LPTLowLevelInterface> LPTWinLowLevel::createInstance(
		std::string platform, uint32_t base_address) {
	if (platform != InterfaceClass::WIN_PLATFORM_NAME) {
		throw UnsupportedPlatformException("Unsupported platform: " + platform);
	}

	LPTWinLowLevel *instance = new LPTWinLowLevel();
	instance->init(base_address);

	return unique_ptr<LPTWinLowLevel>(instance);
}


void LPTWinLowLevel::writeByte(uint32_t address, uint8_t data) {
#ifdef __WIN32__
	Out32(address, data);
#else
	throw UnsupportedPlatformException("Not compiled for Windows platform");
#endif
}


uint8_t LPTWinLowLevel::readByte(uint32_t address) {
#ifdef __WIN32__
	Inp32(address);
#else
	throw UnsupportedPlatformException("Not compiled for Windows platform");
#endif
}


void LPTWinLowLevel::init(uint32_t base_address) {
#ifdef __WIN32__
  if (base_address == 0x3000) {
		unsigned char valor = readByte(0x3008+2);
		printf("ECR: %x\n", valor);
		writeByte(0x3008+2, valor & 0x0f);
	}
	else
	{
		unsigned char valor = readByte(base_address + 0x402);
		printf("ECR: %x\n", valor);
		writeByte(base_address + 0x402, valor & 0x0f);
	}
#endif
}

/*
 * LPTWinLowLevel.cc
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */




