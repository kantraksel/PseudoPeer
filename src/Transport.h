#pragma once
#include <functional>
#include "Connection.h"

constexpr int SLOTS = 4;
constexpr short TIMEOUT = 60;

class Transport
{
	private:
		Thread& thread;

		Connection users[SLOTS];
		unsigned short connectedCount;
		unsigned short nearestFreeSlot;

		void HandlePacket(unsigned int receiverId, const char* buff, int bufflen, WNET::Endpoint& data);
		Connection* GetSender(WNET::Endpoint& data);
		
		void KickInternal(Connection& conn, unsigned int id);
		void UpdateTimers();
	
	public:
		Transport(Thread& thread);
		void Reset();

		bool Prepare();
		void ListenLoop();
		void Shutdown();

		bool Kick(unsigned int id);

		unsigned short GetConnectionCount();
		void ForEachConnection(std::function<void(Connection&)> callback);
};
