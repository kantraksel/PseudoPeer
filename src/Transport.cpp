#include "Logger.h"
#include "Server.h"

Transport::Transport(Thread& thread) : thread(thread)
{
	connectedCount = 0;
	nearestFreeSlot = 0;
	memset(sockets, 0, sizeof(sockets)/sizeof(*sockets));
}

Transport::~Transport()
{

}

void Transport::Reset()
{
	connectedCount = 0;
	for (int i = 0; i < SLOTS; ++i)
	{
		users[i].OnDisconnect();
	}
}

bool Transport::Kick(unsigned int id)
{
	auto& conn = users[id];
	if (conn.IsConnected())
	{
		KickInternal(conn, id);
		return true;
	}
	return false;
}

void Transport::KickInternal(Connection& conn, unsigned int id)
{
	conn.OnDisconnect();
	--connectedCount;
	if (nearestFreeSlot > id) nearestFreeSlot = id;
}

bool Transport::HasAnyConnection()
{
	return connectedCount > 0;
}

unsigned short Transport::GetConnectionCount()
{
	return connectedCount;
}

void Transport::ForEachConnection(std::function<void(Connection&)> callback)
{
	for (int i = 0; i < SLOTS; ++i)
	{
		Connection& conn = users[i];
		if (conn.IsConnected())
			callback(conn);
	}
}

static void PrintNewPeer(WNET::PeerData& data, unsigned int id)
{
	WNET::PeerInfo info;
	WNET::Subsystem::GetPeerInfo(data, info);
	Logger::Log(std::string(info.addr) + ':' + std::to_string(info.port) + " (" + std::to_string(data.address) + ':' + std::to_string(data.port) + ')' + " - connected. Assigned ID: " + std::to_string(id));
}

Connection* Transport::GetSender(WNET::PeerData& data)
{
	Connection* pSender = nullptr;
	for (int i = 0; i < SLOTS; ++i)
	{
		pSender = users + i;
		if (*pSender == data)
			break;
	}

	if (!pSender && connectedCount < SLOTS)
	{
		pSender = users + nearestFreeSlot;
		
		pSender->OnConnect(data);
		++connectedCount;
		PrintNewPeer(data, pSender->GetID());

		for (int i = nearestFreeSlot + 1; i < SLOTS; ++i)
		{
			if (!users[i].IsConnected())
			{
				nearestFreeSlot = i;
				break;
			}
		}
	}

	return pSender;
}

bool Transport::Prepare()
{
	bool bound = true;
	for (int i = 0; i < SLOTS; ++i)
	{
		auto* pSocket = sockets[i] = WNET::IUDPSocket::Create();
		unsigned short port = 7777 + i;
		if (!pSocket->BindSocket("0.0.0.0", port))
		{
			Logger::Log("Failed to bind at 0.0.0.0:" + std::to_string(port));
			bound = false;
			break;
		}
		
		pSocket->SetBlockingMode(false);
		users[i].Setup(pSocket, i);
	}

	return bound;
}

void Transport::ListenLoop()
{
	char buffer[1500];
	WNET::PollFD fds[SLOTS];
	for (int i = 0; i < SLOTS; ++i)
	{
		fds[i].fd = sockets[i]->GetSocket();
	}
	nearestFreeSlot = 0;

	WNET::PeerData data;
	while (!thread.IsStopping())
	{
		int nevents = WNET::PollFD::Poll(fds, SLOTS, 1000);
		if (nevents > 0)
		{
			for (int i = 0; i < SLOTS; ++i)
			{
				auto& fd = fds[i];
				if (fd.isSignaled())
				{
					fd.clearSignal();
					int receivedSize = sockets[i]->ReceiveFrom(buffer, sizeof(buffer), data);
					if (receivedSize > 0) HandlePacket(i, buffer, receivedSize, data);
				}
			}
		}
		CheckTimers();
	}
}

void Transport::HandlePacket(unsigned int receiverId, const char* buff, int bufflen, WNET::PeerData& data)
{
	Connection* pSender = GetSender(data);
	if (pSender)
	{
		pSender->ticks = 0;
		Connection& receiver = users[receiverId];
		if (receiver.IsConnected())
		{
			data.address = receiver.GetIP();
			data.port = receiver.GetPort();
			pSender->GetSocket()->SendTo(buff, bufflen, data);
		}
	}
}

void Transport::Shutdown()
{
	for (int i = 0; i < SLOTS; ++i)
	{
		WNET::IUDPSocket* pSocket = sockets[i];
		if (pSocket)
			delete pSocket;
		sockets[i] = nullptr;
	}
}

void Transport::CheckTimers()
{
	static unsigned long long totalUpdateTime = 0;
	static unsigned long long lastTime = 0;

	unsigned long long nowTime = std::chrono::steady_clock::now().time_since_epoch().count();
	totalUpdateTime += nowTime - lastTime;
	lastTime = nowTime;

	constexpr unsigned long long SecondInNano = 1000000000;
	if (totalUpdateTime >= SecondInNano)
	{
		totalUpdateTime = 0;
		UpdateTimers();
	}
}

void Transport::UpdateTimers()
{
	for (int i = 0; i < SLOTS; ++i)
	{
		Connection& conn = users[i];
		if (!conn.IsConnected()) continue;
		unsigned short ticks = conn.ticks;
		if (ticks == MAX_USHORT) continue;
		
		if (ticks++ == TIMEOUT)
		{
			KickInternal(users[i], i);
			Logger::Log(std::to_string(i) + " timed out");
		}
		else conn.ticks = ticks;
	}
}
