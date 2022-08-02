#pragma once
#include <string>

class Logger
{
	public:
		static void Log(const char* c);
		static void Log(std::string c);
};
