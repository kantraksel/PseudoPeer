#pragma once
#include <thread>
#include <condition_variable>

class Server;

class Thread
{
	private:
		std::thread* hThread;
		Server* pServer;

		std::mutex iMutex;
		std::unique_lock<std::mutex> cvMutex;
		std::condition_variable cv;
		bool isThreadInit;
		bool shutdown;

	public:
		Thread(Server* pServer);
		~Thread();

		void Start();
		void Stop();

		void NotifyThreadReady();
		bool IsStopping();
};
