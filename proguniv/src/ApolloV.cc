#include "../include/ChipIF.hh"
#include "../include/ApolloV.hh"
#include "../include/ATMELGenericSerialHwIF.hh"
#include "../include/ATMELGenericSerialIF.hh"
#include "../include/LPTInterfaceControl.hh"
#include <string.h>
#include <stdio.h>

#define CLK_BIT   3
#define RESET_BIT 4
#define MOSI_BIT  5
#define SCK_BIT   6

ApolloV::ApolloV() {
}

ApolloV::~ApolloV() {
}
	
shared_ptr<Programmer> ApolloV::create(string prog_name, string manufacturer_name, string chip_ref)
{
	using InterfaceClass::ATMELGenericSerialIF;
	using InterfaceClass::ChipIF;
	
	shared_ptr<InterfaceClass::ChipIF> chip = ChipIF::createInstance(manufacturer_name, chip_ref);
	if (chip == 0) {
		//TODO throw chip not found or chip not supported
		return 0;
	}
		
	ApolloV *progPtr = new ApolloV();
	shared_ptr<ApolloV> prog = shared_ptr<ApolloV>(progPtr);
	prog->attachVirtualProgrammer( ATMELGenericSerialIF::createVirtualProgrammer(static_pointer_cast<InterfaceClass::ATMELGenericSerialHwIF,ApolloV>(prog), chip) );
    return prog;
}


void ApolloV::attachVirtualProgrammer(shared_ptr<Programmer> programmer) {
	this->genericAtmelProgrammer = programmer;
}
		
//ATMELGenericSerialIF Interface inherited methods
void ApolloV::setMOSI(unsigned char value)  {
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptBaseAddress);
	lpt.setDataBit(MOSI_BIT, value);
}
		 		
unsigned char ApolloV::getMISO() {
	int value;
	
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptBaseAddress);
	value = lpt.readLPTPin(LPTInterfaceControl::lptStatusnACK);
	
	return (value & 0x01);
}
		 		
void ApolloV::setSCK(unsigned char value) {
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptBaseAddress);
	lpt.setDataBit(SCK_BIT, value);
}	
		 		
void ApolloV::setPOWER(unsigned char value) {
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptBaseAddress);
	if (value == 0) {
		lpt.sendByte( lpt.readByte() & (~0x87) );
	}
	else {
		lpt.sendByte( lpt.readByte() | 0x87 );
	}
}
		 		
void ApolloV::setRESET(unsigned char value) {
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptBaseAddress);
	lpt.setDataBit(RESET_BIT, value);
}
	
void ApolloV::setCLK(unsigned char value) {
	LPTInterfaceControl& lpt = LPTInterfaceControl::getInstance(lptBaseAddress);
	lpt.setDataBit(CLK_BIT, value);
}
	 		
//Programmer Class inherited methods
bool ApolloV::checkChipSignature(bool *isLocked) {
	return genericAtmelProgrammer->checkChipSignature(isLocked);
}

void ApolloV::writeMemory(const std::string &memoryId, const bool verifyWhileWrite, const bool isChipErased) {
	genericAtmelProgrammer->writeMemory(memoryId, verifyWhileWrite, isChipErased);
}

void ApolloV::readMemory(const std::string &memoryId) {
	genericAtmelProgrammer->readMemory(memoryId);
}

void ApolloV::eraseMemory(const std::string &memoryId) {
	genericAtmelProgrammer->eraseMemory(memoryId);
}

void ApolloV::writeConfiguration() {
	genericAtmelProgrammer->writeConfiguration();
}

void ApolloV::readConfiguration() {
	genericAtmelProgrammer->readConfiguration();
}

void ApolloV::eraseConfiguration() {
	genericAtmelProgrammer->eraseConfiguration();
}

void ApolloV::eraseAll() {
	genericAtmelProgrammer->eraseAll();
}

InterfaceClass::ChipIF *ApolloV::getChip() {
	return genericAtmelProgrammer->getChip();
}

void ApolloV::setParameter(const std::string &parameter, const std::string &value) {
	int res;
	
	if ( !parameter.compare( "LPTBaseAddress" ) ) {
		res = sscanf(value.data(), "%x", &lptBaseAddress);
		if (res == EOF || res == 0) {
			throw InvalidParameterValueException(
					"Argumento para parâmetro LPTBaseAddress deve ser um valor hexadecimal começado por 0x\n");
		}
	}
	else {
		genericAtmelProgrammer->setParameter(parameter, value);
	}
}	
