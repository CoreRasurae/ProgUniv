/*
 * UnsupportedPlatformException.hh
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */

#ifndef UNSUPPORTEDPLATFORMEXCEPTION_HH_
#define UNSUPPORTEDPLATFORMEXCEPTION_HH_

#include <string.h>
#include <iostream>
#include <exception>
using namespace std;

class UnsupportedPlatformException: public exception
{
	public:

	UnsupportedPlatformException(const char *newMsg) {
			msg = 0;
			if (newMsg != 0) {
			        int len = strlen(newMsg);
			 	msg = new char[len + 1];
				strncpy( msg, newMsg, len );
			}
		}

	UnsupportedPlatformException(const std::string &newMsg)
		{
			msg = new char[newMsg.length() + 1];
			strncpy( msg, newMsg.data(), newMsg.length() );
		}

		virtual ~UnsupportedPlatformException() throw() {
			if (msg != 0) {
				delete[] msg;
				msg = 0;
			}
		}

		virtual const char *what() const throw()
		{
			return msg;
		}

	private:
		char *msg;
};


#endif /* UNSUPPORTEDPLATFORMEXCEPTION_HH_ */
