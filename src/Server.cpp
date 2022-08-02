#include "Logger.h"
#include "Server.h"

Server::Server() : transport(thread), thread(this)
{
	WNET::Subsystem::Initialize();
}

Server::~Server()
{
	Stop();

	WNET::Subsystem::Release();
}

void Server::Restart()
{
	Stop();
	transport.Reset();
	Start();
}

void Server::Start()
{
	thread.Start();
}

void Server::Stop()
{
	thread.Stop();
}

void Server::ServerThread()
{
	Logger::Log("Starting server...");
	bool hasPrepared = transport.Prepare();
	thread.NotifyThreadReady();

	if (hasPrepared)
	{
		Logger::Log("Server has been started!");
		transport.ListenLoop();
		Logger::Log("Stopping server...");
	}
	
	transport.Shutdown();
	Logger::Log("Server stopped");
}

void Server::KickAll()
{
	if (transport.HasAnyConnection())
		for (int i = 0; i < SLOTS; ++i)
			Kick(i);
}

void Server::Kick(unsigned int id)
{
	if (id >= SLOTS)
		Logger::Log("Invalid ID");
	else if (transport.Kick(id))
		Logger::Log(std::to_string(id) + " has been kicked!");
}

void Server::PrintStatus()
{
	WNET::PeerInfo info;
	WNET::PeerData data;
	Logger::Log("Currently connected users: " + std::to_string(transport.GetConnectionCount()));
	transport.ForEachConnection([&](Connection& conn)
	{
		data.address = conn.GetIP();
		data.port = conn.GetPort();
			
		WNET::Subsystem::GetPeerInfo(data, info);
		Logger::Log(" - ID: " + std::to_string(conn.GetID()) + " - ADDRESS: " + std::string(info.addr) + ':' + std::to_string(info.port));
	});
}
