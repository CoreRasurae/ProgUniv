#ifndef __NOTIMPLEMENTEDEXCEPTION_HH__
#define __NOTIMPLEMENTEDEXCEPTION_HH__

#include "CommandException.hh"

class NotImplementedException: public CommandException
{
	public:
		NotImplementedException(const std::string &newMsg) : CommandException(newMsg) {

		}
};

#endif //__NOTIMPLEMENTEDEXCEPTION_HH__
