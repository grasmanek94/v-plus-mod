#include "Networking.hxx"

int Packet::Send(NetworkClient& client)
{
	if (!packet)
	{
		return 0;
	}

	int return_value = client.Send(packet);
	packet = nullptr;
	return return_value;
}

int Packet::Send(NetworkServer& server, ENetPeer* peer)
{
	if (!packet)
	{
		return 0;
	}

	int return_value = server.Send(peer, packet);
	packet = nullptr;
	return return_value;
}

void Packet::Broadcast(NetworkServer& server)
{
	if (!packet)
	{
		return;
	}

	server.Broadcast(packet);
}
