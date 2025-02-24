#pragma once
#include <condition_variable>
#include <functional>
#include <thread>

class Thread
{
	private:
		std::jthread thread;
		std::mutex mutex;
		std::condition_variable cv;
		bool isReady;
		
	public:
		typedef std::function<void()> StartRoutine;

		Thread();
		~Thread();

		void Start(StartRoutine routine);
		void Stop();

		void NotifyReady();
		bool IsStopping();
};
