#include "Logger.h"
#include "Server.h"

Server::Server() : transport(thread)
{
	WNET::Subsystem::Initialize();
}

Server::~Server()
{
	Stop();

	WNET::Subsystem::Release();
}

void Server::Start()
{
	thread.Start(std::bind(&Server::ServerThread, this));
}

void Server::Restart()
{
	Stop();
	transport.Reset();
	Start();
}

void Server::Stop()
{
	thread.Stop();
}

void Server::ServerThread()
{
	Logger::Log("Starting server...");
	bool hasPrepared = transport.Prepare();
	thread.NotifyReady();

	if (hasPrepared)
	{
		Logger::Log("Server has been started!");
		transport.ListenLoop();
		Logger::Log("Stopping server...");
		KickAll();
	}
	
	transport.Shutdown();
	Logger::Log("Server stopped");
}

void Server::KickAll()
{
	transport.ForEachConnection([&](const Connection& conn)
		{
			Kick(conn.GetID());
		});
}

void Server::Kick(unsigned int id)
{
	if (transport.Kick(id))
		Logger::Log(std::to_string(id) + " has been kicked!");
}

void Server::PrintStatus()
{
	Logger::Log("Currently connected users: " + std::to_string(transport.GetConnectionCount()));
	transport.ForEachConnection([&](Connection& conn)
		{
			auto info = conn.GetSocket().GetPeerAddress();
			Logger::Log(" - ID: " + std::to_string(conn.GetID()) + " - ADDRESS: " + std::string(info.address) + ':' + std::to_string(info.port));
		});
}
