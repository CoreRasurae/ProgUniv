#ifndef _LPTLOWLEVELINTERFACE_HH_
#define _LPTLOWLEVELINTERFACE_HH_

#include <cinttypes>
#include <memory>
#include <string>
#include "UnsupportedPlatformException.hh"

using namespace std;

namespace InterfaceClass {
   static std::string WIN_PLATFORM_NAME = std::string("WINDOWS");
   static std::string LINUX_PLATFORM_NAME = std::string("LINUX");

	class LPTLowLevelInterface {
		public:
			static unique_ptr<LPTLowLevelInterface> createInstance(std::string platform, uint32_t base_address);
			virtual void writeByte(uint32_t address, uint8_t data) = 0;
			virtual uint8_t readByte(uint32_t address) = 0;
			virtual ~LPTLowLevelInterface() {}

		protected:
			LPTLowLevelInterface() {}
			LPTLowLevelInterface(const LPTLowLevelInterface& rhs) {}

		private:
			LPTLowLevelInterface& operator=(const LPTLowLevelInterface& rhs);

	};
}
#endif
