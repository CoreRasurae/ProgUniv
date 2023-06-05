/*
 * UnsupportedParameterException.hh
 *
 *  Created on: 15 de Mai de 2013
 *      Author: lpnm
 */

#ifndef UNSUPPORTEDPARAMETEREXCEPTION_HH_
#define UNSUPPORTEDPARAMETEREXCEPTION_HH_

#include "CommandException.hh"

class UnsupportedParameterException: public CommandException
{
	public:
		UnsupportedParameterException(const std::string &newMsg) : CommandException(newMsg) {

		}
};


#endif /* UNSUPPORTEDPARAMETEREXCEPTION_HH_ */
