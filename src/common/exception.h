#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>

class Exception : public std::runtime_error
{
public:
	Exception(const char *message="foo") : std::runtime_error(message)
	{
	}
};
#endif EXCEPTION_H

