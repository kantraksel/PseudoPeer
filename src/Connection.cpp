#include "wnet.h"
#include "Connection.h"

Connection::Connection()
{
	id = 0;
	ip = 0;
	port = 0;
	isConnected = false;
	pSocket = nullptr;
}

void Connection::Setup(WNET::IUDPSocket* pSocket, unsigned int id)
{
	this->pSocket = pSocket;
	this->id = id;
}

unsigned int Connection::GetID() const
{
	return id;
}

unsigned int Connection::GetIP() const
{
	return ip;
}

unsigned short Connection::GetPort() const
{
	return port;
}

bool Connection::IsConnected() const
{
	return isConnected;
}

WNET::IUDPSocket* Connection::GetSocket() const
{
	return pSocket;
}

void Connection::OnConnect(const WNET::PeerData& data)
{
	ip = data.address;
	port = data.port;
	isConnected = true;
	ticks = 0;
}

void Connection::OnDisconnect()
{
	isConnected = false;
}

bool Connection::operator==(const WNET::PeerData& data)
{
	return this->isConnected && this->ip == data.address && this->port == data.port;
}
