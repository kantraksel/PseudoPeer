#include "Connection.h"

Connection::Connection()
{
	isConnected = false;
	id = 0;
	lastReceive = 0;
}

void Connection::Setup(unsigned int id)
{
	this->id = id;
}

unsigned int Connection::GetID() const
{
	return id;
}

bool Connection::IsConnected() const
{
	return isConnected;
}

void Connection::OnConnect()
{
	lastReceive = 0;
	isConnected = true;
}

void Connection::OnDisconnect()
{
	isConnected = false;
}

WNET::UdpSocket& Connection::GetSocket()
{
	return socket;
}

long long Connection::GetLastReceive() const
{
	return lastReceive;
}

void Connection::SetLastReceive(long long time)
{
	lastReceive = time;
}

bool Connection::operator==(const WNET::Endpoint& data)
{
	auto& peer = socket.GetPeer();
	return this->isConnected && peer.address == data.address && peer.port == data.port;
}
