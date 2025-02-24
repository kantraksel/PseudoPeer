#include "Logger.h"
#include "Thread.h"
#include "Transport.h"

Transport::Transport(Thread& thread) : thread(thread)
{
	connectedCount = 0;
	nearestFreeSlot = 0;
}

void Transport::Reset()
{
	nearestFreeSlot = 0;
	connectedCount = 0;
	for (int i = 0; i < SLOTS; ++i)
	{
		users[i].OnDisconnect();
	}
}

// main functions
bool Transport::Prepare()
{
	bool bound = true;
	for (int i = 0; i < SLOTS; ++i)
	{
		auto& socket = users[i].GetSocket();
		unsigned short port = 7741 + i;
		if (!socket.Create() || !socket.Bind("0.0.0.0", port))
		{
			Logger::Log("Failed to bind at 0.0.0.0:" + std::to_string(port));
			bound = false;
			break;
		}
		
		users[i].Setup(i);
	}

	return bound;
}

void Transport::ListenLoop()
{
	char buffer[1500];
	WNET::PollFD fds[SLOTS];
	for (int i = 0; i < SLOTS; ++i)
	{
		fds[i].fd = users[i].GetSocket().GetSocket();
	}

	WNET::Endpoint data;
	while (!thread.IsStopping())
	{
		int nevents = WNET::PollFD::Poll(fds, SLOTS, 100);
		if (nevents > 0)
		{
			for (int i = 0; i < SLOTS; ++i)
			{
				auto& fd = fds[i];
				if (fd.isSignaled())
				{
					fd.clearSignal();

					while (true)
					{
						int receivedSize = users[i].GetSocket().ReceiveFrom(buffer, sizeof(buffer), data);
						if (receivedSize > 0)
							HandlePacket(i, buffer, receivedSize, data);
						else break;
					}
				}
			}
		}

		UpdateTimers();
	}
}

void Transport::Shutdown()
{
	for (int i = 0; i < SLOTS; ++i)
	{
		auto& socket = users[i].GetSocket();
		socket.Close();
	}
}

// Kick
bool Transport::Kick(unsigned int id)
{
	if (id >= SLOTS)
		return false;

	auto& conn = users[id];
	if (!conn.IsConnected())
		return false;
	
	KickInternal(conn, id);
	return true;
}

void Transport::KickInternal(Connection& conn, unsigned int id)
{
	conn.OnDisconnect();
	--connectedCount;

	if (nearestFreeSlot > id)
		nearestFreeSlot = id;
}

// Utilities
unsigned short Transport::GetConnectionCount()
{
	return connectedCount;
}

void Transport::ForEachConnection(std::function<void(Connection&)> callback)
{
	for (int i = 0; i < SLOTS; ++i)
	{
		auto& conn = users[i];
		if (conn.IsConnected())
			callback(conn);
	}
}

// private utils
static void PrintNewPeer(WNET::Endpoint& data, unsigned int id)
{
	auto info = data.ToAddress();
	Logger::Log(std::string(info.address) + ':' + std::to_string(info.port) + " - connected. Assigned ID: " + std::to_string(id));
}

Connection* Transport::GetSender(WNET::Endpoint& data)
{
	for (int i = 0; i < SLOTS; ++i)
	{
		auto* pConn = users + i;
		if (*pConn == data)
			return pConn;
	}

	if (connectedCount < SLOTS)
	{
		++connectedCount;

		auto* pConn = users + nearestFreeSlot;
		pConn->OnConnect();

		for (int i = nearestFreeSlot + 1; i < SLOTS; ++i)
		{
			if (!users[i].IsConnected())
			{
				nearestFreeSlot = i;
				break;
			}
		}

		PrintNewPeer(data, pConn->GetID());
		return pConn;
	}

	return nullptr;
}

void Transport::HandlePacket(unsigned int receiverId, const char* buff, int bufflen, WNET::Endpoint& data)
{
	auto* pSender = GetSender(data);
	if (pSender)
	{
		pSender->SetLastReceive(std::chrono::steady_clock::now().time_since_epoch().count());

		auto& receiver = users[receiverId];
		if (receiver.IsConnected())
		{
			data = receiver.GetSocket().GetPeer();
			pSender->GetSocket().SendTo(buff, bufflen, data);
		}
	}
}

// Timers
void Transport::UpdateTimers()
{
	constexpr long long Timeout = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(TIMEOUT)).count();
	auto now = std::chrono::steady_clock::now().time_since_epoch().count();

	for (int i = 0; i < SLOTS; ++i)
	{
		auto& conn = users[i];
		if (!conn.IsConnected())
			continue;
		
		if ((now - conn.GetLastReceive()) >= Timeout)
		{
			KickInternal(users[i], i);
			Logger::Log(std::to_string(i) + " timed out");
		}
	}
}
