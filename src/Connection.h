#pragma once
#include "wnet.h"

class Connection
{
	private:
		WNET::UdpSocket socket;
		bool isConnected;
		unsigned char id;
		long long lastReceive;

	public:
		Connection();
		void Setup(unsigned int id);

		bool IsConnected() const;
		void OnConnect();
		void OnDisconnect();

		unsigned int GetID() const;
		WNET::UdpSocket& GetSocket();
		long long GetLastReceive() const;
		void SetLastReceive(long long time);

		bool operator==(const WNET::Endpoint& data);
};
