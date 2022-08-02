#pragma once
#include "wnet.h"
#include "Connection.h"
#include "Thread.h"
#include "Transport.h"

class Server
{
	friend Thread;
	private:
		Transport transport;
		Thread thread;

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
