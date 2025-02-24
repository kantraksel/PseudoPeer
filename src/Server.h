#pragma once
#include "Thread.h"
#include "Transport.h"

class Server
{
	private:
		Thread thread;
		Transport transport;
		
		void ServerThread();

	public:
		Server();
		~Server();

		void Start();
		void Stop();
		void Restart();

		void KickAll();
		void Kick(unsigned int id);
		void PrintStatus();
};
