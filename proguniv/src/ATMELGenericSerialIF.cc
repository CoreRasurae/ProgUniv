#include "../include/ATMELGenericSerialIF.hh"
#include "../include/ATMELGenericSerial.hh"
#include "../include/ATMELGenericSerialPagedMem.hh"
#include "../include/Property.hh"
#include "../include/ErrorBuildingProgrammerException.hh"

using namespace std;

namespace InterfaceClass {
	shared_ptr<Programmer> ATMELGenericSerialIF::createVirtualProgrammer(shared_ptr<InterfaceClass::ATMELGenericSerialHwIF> realProg, shared_ptr<InterfaceClass::ChipIF> targetChip) {
		try {
			Property &prop = targetChip->getProgrammerConfigurationProperty("MemoryType");
			
			if ( prop.getValue().compare("NonPaged") == 0 )
				return shared_ptr<Programmer>(new ATMELGenericSerial(realProg, targetChip));
			else
				return shared_ptr<Programmer>(new ATMELGenericSerialPagedMem(realProg, targetChip));
		} catch (InvalidPropertyNameException e) {
			throw ErrorBuildingProgrammerException("Unable to build programmer object: " + std::string( e.what() ) );
		}
	}
}
