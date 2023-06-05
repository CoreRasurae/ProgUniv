/*
 * LPTLowLevelInterface.cc
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */
#include "../include/LPTLowLevelInterface.hh"
#include "../include/LPTWinLowLevel.hh"
#include "../include/LPTLinuxLowLevel.hh"

using namespace InterfaceClass;

unique_ptr<LPTLowLevelInterface> LPTLowLevelInterface::createInstance(
		std::string platform, uint32_t base_address) {
	if (platform == LINUX_PLATFORM_NAME) {
		return LPTLinuxLowLevel::createInstance(platform, base_address);
	} else if (platform == WIN_PLATFORM_NAME) {
		return LPTWinLowLevel::createInstance(platform, base_address);
	} else {
		throw UnsupportedPlatformException("Platform " + platform + " is unsupported");
	}
}




