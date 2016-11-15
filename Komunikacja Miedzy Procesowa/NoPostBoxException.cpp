#include "NoPostBoxException.h"




const char* NoPostBoxException::what() const noexcept
{
		return "There is no postbox for this PID\n";
}

NoPostBoxException::NoPostBoxException()
{}


NoPostBoxException::~NoPostBoxException()
{}
