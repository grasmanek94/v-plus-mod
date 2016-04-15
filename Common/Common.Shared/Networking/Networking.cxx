#include "Networking.hxx"

#ifdef _WIN32

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<EventConnect>& data)
{
	EventConnect_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<EventDisconnect>& data)
{
	EventDisconnect_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<PeerConnected>& data)
{
	PeerConnected_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<PeerDisconnected>& data)
{
	PeerDisconnected_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<ChatMessage>& data)
{
	ChatMessage_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<PlayerJoin>& data)
{
	PlayerJoin_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<PlayerQuit>& data)
{
	PlayerQuit_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<PlayerSpawn>& data)
{
	PlayerSpawn_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<PlayerDespawn>& data)
{
	PlayerDespawn_handler.Add(data);
}

void V_Plus_NetworkClient::Handle(ENetPeer* peer, const std::shared_ptr<OnFootSync>& data)
{
	//if(GTA_Active())
	//{
		OnFootSync_handler.Add(data);
	//}
}

void V_Plus_NetworkClient::RunAsync()
{
	ENetPacket* packet = nullptr;
	while (delayed_packets_to_send.try_pop(packet))
	{
		NetworkClient::Send(packet);
	}

	if (NetworkClient::Pull())
	{
		ProcessEvent(NetworkClient::Event());
	}
}


#endif
