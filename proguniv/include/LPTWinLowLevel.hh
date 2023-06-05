/*
 * LPTWinLowLevel.hh
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */

#ifndef LPTWINLOWLEVEL_HH_
#define LPTWINLOWLEVEL_HH_

#include <memory>
#include "../include/LPTLowLevelInterface.hh"

class LPTWinLowLevel : public InterfaceClass::LPTLowLevelInterface {
public:
	static unique_ptr<LPTLowLevelInterface> createInstance(std::string platform, uint32_t address);

	virtual void writeByte(uint32_t address, uint8_t data);
    virtual uint8_t readByte(uint32_t address);

private:
    void init(uint32_t base_address);
};


#endif /* LPTWINLOWLEVEL_HH_ */
