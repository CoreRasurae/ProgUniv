/*
 * LPTLinuxLowLevel.cc
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */
#include "../include/LPTLinuxLowLevel.hh"
#include <cinttypes>
#ifdef __linux__
#include <unistd.h>
#include <sys/io.h>
#include <stdlib.h>
#include <stdint.h>
#define extern static
//Only for kernels < 2.6.19
//#include <asm/io.h>
//for kernels >= 2.6.19 use sys/io.h
#undef extern
#endif
#include <iostream>

using std::ios;

using namespace std;


unique_ptr<InterfaceClass::LPTLowLevelInterface> LPTLinuxLowLevel::createInstance(
		std::string platform, uint32_t base_address) {
#ifdef __linux__
	if (platform != InterfaceClass::LINUX_PLATFORM_NAME) {
		throw UnsupportedPlatformException("Unsupported platform: " + platform);
	}

	LPTLinuxLowLevel *instance = new LPTLinuxLowLevel();
	instance->init(base_address);

	return unique_ptr<LPTLinuxLowLevel>(instance);
#else
	throw UnsupportedPlatformException("Linux platform is disabled");
#endif
}

void LPTLinuxLowLevel::init(uint32_t base_address) {
#ifdef __linux__
	/* Get access to the ports */
	//if (base_lpt > 0x3ff) {
		if ( iopl(1) < 0 ) {
	    	perror("iopl");
	    	exit(1);
		}
	//}

    if (ioperm(base_address, 3, 1))
    {
    	perror("ioperm");
    	exit(1);
    }

    cout << "Acquired address region for " << base_address << endl;
    //Initialization in case of an ECP port
    //Escrever para o endereço base_lpt + 0x400, o valor 00100000, ou 0x20
    /* Get access to the ports */

    if (base_address == 0x3000) {
        if (ioperm(0x3008, 3, 1))
        {
        	perror("ioperm");
        	exit(1);
        }

        unsigned char valor = inb(0x3008+2);
        printf("ECR: %x\n", valor);
        outb(valor & 0x0f, 0x3008+2);
    }
    else
    {
    	if (ioperm(base_address + 0x400, 3, 1))
    	{
    	  	perror("ioperm");
    	   	exit(1);
    	}

    	unsigned char valor = inb(base_address + 0x402);
        printf("ECR: %x\n", valor);
        outb(valor & 0x0f, base_address + 0x402);
    }
#endif
}

void LPTLinuxLowLevel::writeByte(uint32_t address, uint8_t data) {
#ifdef __linux__
	outb(data, address);
#endif
}

uint8_t LPTLinuxLowLevel::readByte(uint32_t address) {
#ifdef __linux__
	return inb(address);
#endif
}
