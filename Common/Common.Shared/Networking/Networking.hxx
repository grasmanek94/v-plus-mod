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
		size_t unique_id = object.UniqueClassId();
		ss.write(reinterpret_cast<char*>(&unique_id), sizeof(size_t));
		cereal::BinaryOutputArchive oarchive(ss);
		oarchive(object);

		size_t x = ss.tellp();

		packet = enet_packet_create(nullptr, x, flags);

		ss.rdbuf()->sgetn(reinterpret_cast<char*>(packet->data), x);
	}

	int Send(NetworkClient& client);
	int Send(NetworkServer& server, ENetPeer* peer);
	void Broadcast(NetworkServer& server);
};

#include "Messages.hxx"

class MessageReceiver
{
private:
	virtual void Handle(const ENetPeer* peer, PeerConnected& data) = 0;
	virtual void Handle(const ENetPeer* peer, PeerDisconnected& data) = 0;
	virtual void Handle(const ENetPeer* peer, ChatMessage& message) = 0;
	virtual void Handle(const ENetPeer* peer, PlayerJoin& message) = 0;
	virtual void Handle(const ENetPeer* peer, PlayerQuit& message) = 0;
	virtual void Handle(const ENetPeer* peer, PlayerSpawn& message) = 0;
	virtual void Handle(const ENetPeer* peer, PlayerDespawn& message) = 0;
	virtual void Handle(const ENetPeer* peer, OnFootSync& message) = 0;

public:
	template <typename size_t unused = 0>
	void ProcessPacketReceiveEvent(ENetEvent& event)
	{
		//Handle packet
		ENetPacket* packet = event.packet;
		if (packet->dataLength >= sizeof(size_t))
		{
			size_t unique_class_id = (*reinterpret_cast<size_t*>(packet->data));

			#define IMPLEMENT_CASE_FOR(class_name) \
				case class_name::UniqueClassId(): \
				{ \
					bool errorOccured = false; \
					class_name var; \
					std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary); \
					\
					if (packet->dataLength > sizeof(size_t)) \
					{ \
						stream.write(reinterpret_cast<char*>(packet->data + sizeof(size_t)), packet->dataLength - sizeof(size_t)); \
					} \
					\
					try \
					{ \
						cereal::BinaryInputArchive iarchive(stream); \
						iarchive(var); \
					} \
					catch (const std::exception&) \
					{ \
						errorOccured = true; \
					} \
					\
					if (!errorOccured) \
					{ \
						Handle(event.peer, var); \
					} \
				} \
				break;

			#pragma warning( push )
			#pragma warning( disable : 4307 )

			switch (unique_class_id)
			{
				IMPLEMENT_CASE_FOR(ChatMessage);
				IMPLEMENT_CASE_FOR(PlayerJoin);
				IMPLEMENT_CASE_FOR(PlayerQuit);
				IMPLEMENT_CASE_FOR(PlayerSpawn);
				IMPLEMENT_CASE_FOR(PlayerDespawn);
				IMPLEMENT_CASE_FOR(OnFootSync);

				//Because of this we need to impl in header, so, template:
				#ifdef VPLUS_CLIENT
				IMPLEMENT_CASE_FOR(PeerConnected);
				IMPLEMENT_CASE_FOR(PeerDisconnected);
				#endif
			}

			#pragma warning( pop )

			#undef IMPLEMENT_CASE_FOR

		}
		/* Clean up the packet now that we're done using it. */
		enet_packet_destroy(event.packet);
	}
};

class V_Plus_NetworkClient: public NetworkClient
{
public:
	template<typename T>
	int Send(const T& object, _ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE)
	{
		/*
			Well reduced it to double-buffer, I think we're not going to get any faster with this
		*/
		std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
		size_t unique_id = object.UniqueClassId();
		ss.write(reinterpret_cast<char*>(&unique_id), sizeof(size_t));
		cereal::BinaryOutputArchive oarchive(ss);
		oarchive(object);

		size_t x = ss.tellp();
		ENetPacket* packet = enet_packet_create(nullptr, x, flags);

		if (packet)
		{
			ss.rdbuf()->sgetn(reinterpret_cast<char*>(packet->data), x);
			return NetworkClient::Send(packet);
		}
		return -1;
	}
};

class V_Plus_NetworkServer : public NetworkServer
{
public:
	template<typename T>
	int Send(ENetPeer* peer, const T& object, _ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE)
	{
		/*
		Well reduced it to double-buffer, I think we're not going to get any faster with this
		*/
		std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
		size_t unique_id = object.UniqueClassId();
		ss.write(reinterpret_cast<char*>(&unique_id), sizeof(size_t));
		cereal::BinaryOutputArchive oarchive(ss);
		oarchive(object);

		size_t x = ss.tellp();
		ENetPacket* packet = enet_packet_create(nullptr, x, flags);

		if (packet)
		{
			ss.rdbuf()->sgetn(reinterpret_cast<char*>(packet->data), x);
			return NetworkServer::Send(peer, packet);
		}
		return -1;
	}

	template<typename T>
	bool Broadcast(const T& object, _ENetPacketFlag flags = ENET_PACKET_FLAG_RELIABLE)
	{
		/*
		Well reduced it to double-buffer, I think we're not going to get any faster with this
		*/
		std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
		size_t unique_id = object.UniqueClassId();
		ss.write(reinterpret_cast<char*>(&unique_id), sizeof(size_t));
		cereal::BinaryOutputArchive oarchive(ss);
		oarchive(object);

		size_t x = ss.tellp();
		ENetPacket* packet = enet_packet_create(nullptr, x, flags);

		if (!packet)
		{
			return false;
		}

		ss.rdbuf()->sgetn(reinterpret_cast<char*>(packet->data), x);
		NetworkServer::Broadcast(packet);
		return true;
	}
};
