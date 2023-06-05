#ifndef UTILS_HH_
#define UTILS_HH_

#include <string>
#include <algorithm>
#include <cctype>
#include <stdio.h>
#include <string.h>
#include "../include/ConversionException.hh"

class to_lower
{
	public:
	    char operator() (char c) const        // notice the return type
	    {
	        return tolower(c);
	    }
};

class to_upper
{
	public:
	    char operator() (char c) const        // notice the return type
	    {
	        return toupper(c);
	    }
};

class Utils {
	public:
		static int string2int(const std::string &valorStr) { 
			int value, res;
			
			res = sscanf( valorStr.data(), "%d", &value);
			if (res == EOF || res == 0) {
				std::string error = "Unable to execute string2int() conversion for string value: " + valorStr;
				throw new ConversionException(error);
			}
			
			return value;
		}
		
		static bool string2bool(const std::string &valorStr) {
			using namespace std;

			std::string teste(valorStr);
			
			transform (valorStr.begin(), valorStr.end(), teste.begin(), to_upper() );
			
			if ( teste.compare("TRUE") == 0 ) {
				return true;
			}
			else if ( teste.compare("FALSE") == 0 ) {
				return false;
			}
			else {
				throw new ConversionException("Unable to convert from string to bool in string2bool() for string value: " + valorStr);
			}
				
		}
		
		static std::string int2string(const long value) {
			char stringValue[100];
			snprintf(stringValue, 99, "%ld", value);
			
			return std::string(stringValue);
		}
		
		static std::string int2hexString(const long value) {
			char stringValue[20];
			snprintf(stringValue, 19, "0x%lx", value);
			
			return std::string(stringValue);
		}
};

#endif /*UTILS_HH_*/
