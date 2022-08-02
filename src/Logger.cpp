#include "Logger.h"
#include <iostream>

void Logger::Log(const char* c)
{
	std::cout << c << std::endl;
}

void Logger::Log(std::string c)
{
	std::cout << c << std::endl;
}
