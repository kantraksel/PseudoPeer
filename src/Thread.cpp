#include "Server.h"
#include <cassert>

Thread::Thread(Server* pServer) : cvMutex(iMutex), pServer(pServer)
{
	hThread = nullptr;
	isThreadInit = false;
	shutdown = false;
}

Thread::~Thread()
{

}

void Thread::Start()
{
	assert(hThread == nullptr);

	shutdown = false;
	hThread = new std::thread(&Server::ServerThread, pServer);

	cv.wait(cvMutex, [&] { return isThreadInit; });
}

void Thread::Stop()
{
	shutdown = true;
	
	if (hThread)
	{
		hThread->join();
		delete hThread;
		hThread = nullptr;
	}
}

void Thread::NotifyThreadReady()
{
	isThreadInit = true;
	cv.notify_one();
}

bool Thread::IsStopping()
{
	return shutdown;
}
