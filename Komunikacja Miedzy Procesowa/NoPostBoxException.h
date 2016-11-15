#pragma once
#include <exception>
#include <string>

class NoPostBoxException : public std::exception
{

public:
	const char* what() const noexcept override;
	NoPostBoxException();
	~NoPostBoxException();
};

