#pragma once
#define MAX_USHORT (unsigned short)0xFFFFU

class Connection
{
	bool isConnected;
	unsigned char id;
	unsigned int ip;
	unsigned short port;
	WNET::IUDPSocket* pSocket;

	public:
		unsigned short ticks;
		Connection();
		void Setup(WNET::IUDPSocket* pSocket, unsigned int id);

		unsigned int GetID() const;
		unsigned int GetIP() const;
		unsigned short GetPort() const;
		WNET::IUDPSocket* GetSocket() const;

		bool IsConnected() const;
		void OnConnect(const WNET::PeerData& data);
		void OnDisconnect();

		bool operator==(const WNET::PeerData& data);
};
