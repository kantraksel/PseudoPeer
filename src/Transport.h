#pragma once
#include <functional>

constexpr int SLOTS = 4;
constexpr short TIMEOUT = 60;

class Transport
{
	private:
		Thread& thread;
		WNET::IUDPSocket* sockets[SLOTS];
		Connection users[SLOTS];
		unsigned short connectedCount;
		unsigned short nearestFreeSlot;

		void HandlePacket(unsigned int receiverId, const char* buff, int bufflen, WNET::PeerData& data);
		Connection* GetSender(WNET::PeerData& data);
		void KickInternal(Connection& conn, unsigned int id);
		void CheckTimers();
		void UpdateTimers();
	
	public:
		Transport(Thread& thread);
		~Transport();
		void Reset();

		bool Prepare();
		void ListenLoop();
		void Shutdown();

		bool Kick(unsigned int id);

		bool HasAnyConnection();
		unsigned short GetConnectionCount();
		void ForEachConnection(std::function<void(Connection&)> callback);
};
