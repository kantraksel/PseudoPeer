#include "Logger.h"
#include "Server.h"
#include <iostream>
#include "version.h"

Server server;

void CommandLoop()
{
	std::string line;
	while (std::getline(std::cin, line))
	{
		if (line == "stop")
		{
			server.Stop();
			break;
		}
		else if (line == "restart")
		{
			server.Restart();
		}
		else if (line == "kickall")
		{
			server.KickAll();
			Logger::Log("Kicked all!");
		}
		else if (line == "help")
		{
			Logger::Log("Available commands:");
			Logger::Log(" - stop - stops server");
			Logger::Log(" - kickall - kicks all clients");
			Logger::Log(" - kick <id> - kicks the client");
			Logger::Log(" - status - prints slot status");
			Logger::Log(" - restart - restarts server");
		}
		else if (line == "status") server.PrintStatus();
		else
		{
			std::string cmd;
			for (int i = 0; i < line.length(); ++i)
			{
				char c = line[i];
				if (c == ' ')
				{
					line = line.substr(i + 1);
					break;
				}
				cmd += c;
			}
			if (line == cmd) line.clear();

			if (cmd == "kick")
			{
				unsigned int id = 0;
				try
				{
					id = std::stoul(line);
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
					continue;
				}
				
				server.Kick(id);
			}
		}
	}
}

int main()
{
	Logger::Log("PseudoPeer " VERSION);

	server.Start();

	Logger::Log("Type 'help' for help");
	CommandLoop();
}
