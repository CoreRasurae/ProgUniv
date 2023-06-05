#include "../include/ProgrammerFactory.hh"
#include "../include/ApolloV.hh"
#include <string.h>

ProgrammerFactory* ProgrammerFactory::factory = 0;

//Implementação do método
ProgrammerFactory::ProgrammerFactory() { }

ProgrammerFactory::~ProgrammerFactory() { }

ProgrammerFactory& ProgrammerFactory::getFactory() {
	if (factory == 0) {
		factory = new ProgrammerFactory();
	}
	
	return *factory;
}

shared_ptr<Programmer> ProgrammerFactory::createProgrammer(string prog_name, string manufacturer_name, string chip_ref) {
	if ( string("ApolloV").compare(prog_name) == 0 ) {
		return ApolloV::create(prog_name, manufacturer_name, chip_ref);
	}
	else if ( string("CCCP").compare(prog_name) == 0 ) {
		//return new ACCCPFactory();
		return 0;
	}
	else
		return 0;		
}
