#include "Server.h"

Thread::Thread()
{
	isReady = false;
}

Thread::~Thread()
{
	Stop();
}

void Thread::Start(StartRoutine routine)
{
	if (thread.joinable())
		return;
	
	isReady = false;
	std::unique_lock cvMutex(mutex);

	thread = std::jthread(routine);
	cv.wait(cvMutex, [this] { return isReady; });
}

void Thread::Stop()
{
	if (thread.joinable())
	{
		thread.request_stop();
		thread = {};
	}
}

void Thread::NotifyReady()
{
	{
		std::lock_guard lock(mutex);
		isReady = true;
	}
	
	cv.notify_one();
}

bool Thread::IsStopping()
{
	return thread.get_stop_token().stop_requested();
}
