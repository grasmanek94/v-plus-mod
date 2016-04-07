#pragma once

#include <enet/enetpp.hxx>

class Packet
{
private:
	ENetPacket* packet;
public:
	template<typename T>
	Packet(const T& object, _ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE)
	{
		/*
		Well reduced it to double-buffer, I think we're not going to get any faster with this
		*/
		std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
		size_t unique_id = object.Event_Id();
		ss.write(reinterpret_cast<char*>(&unique_id), sizeof(size_t));
		cereal::BinaryOutputArchive oarchive(ss);
		oarchive(object);

		packet = enet_packet_create(nullptr, ss.tellg(), flags);

		ss.rdbuf()->sgetn(reinterpret_cast<char*>(packet->data), ss.tellg());
	}

	int Send(NetworkClient& client);
	int Send(NetworkServer& server, ENetPeer* peer);
	void Broadcast(NetworkServer& server);
};

#include <Messages.hxx>
