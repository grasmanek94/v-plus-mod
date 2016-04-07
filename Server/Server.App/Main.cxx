#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <set>
#include <Common.hxx>
#include <sstream>
#include <exception>
#include <algorithm>
#include <iterator>

#include <Networking.hxx>
class Id
{
	std::vector<size_t> free_ids;
	std::vector<bool> taken_ids;
public:
	Id(size_t max_ids)
	{
		free_ids.reserve(max_ids);
		taken_ids.resize(max_ids);
		for (size_t i = 0; i < max_ids; ++i)
		{
			free_ids.push_back((max_ids -1 ) - i);
		}
	}

	size_t GetId()
	{
		if (free_ids.size())
		{
			size_t free_id = free_ids.back();
			taken_ids[free_id] = true;
			free_ids.pop_back();
			return free_id;
		}
		return (size_t)-1;
	}

	bool FreeId(size_t id)
	{
		if (taken_ids.size() >= id)
		{
			return false;
		}

		if (!taken_ids[id])
		{
			return false;
		}

		taken_ids[id] = false;
		free_ids.push_back(id);

		return true;
	}
};

class Server
{
private:
	NetworkServer connection;
	Id id_generator;
	const size_t max_players;
	std::vector<ENetPeer*> peers;
	std::set<ENetPeer*> connected_peers;

	void HandleTick()
	{

	}

	void Handle(const ENetPeer* peer, PeerConnected& data)
	{

	}

	void Handle(const ENetPeer* peer, PeerDisconnected& data)
	{

	}

	void Handle(const ENetPeer* peer, ChatMessage& message)
	{
		message.SetSender(reinterpret_cast<size_t>(peer->data));

		Packet packet(message);
		packet.Broadcast(connection);
	}

public:
	Server(const std::string bind_address, uint16_t port, size_t max_players)
		: id_generator(max_players), max_players(max_players)
	{
		int init_code = connection.GetInitCode();

		if (init_code)
		{
			// TODO custom exception class
			throw std::exception(("Cannot initialize ENET, error code: " + std::to_string(init_code)).c_str());
		}

		connection.SetHost(bind_address, port);

		if (!connection.Create(max_players) || !connection.Good())
		{
			// TODO custom exception class
			throw std::exception("ENET host member creation failed");
		}		

		peers.resize(max_players);
		//ready
	}

	void Tick()
	{
		if (connection.Pull())
		{
			ENetEvent event = connection.Event();

			switch (event.type)
			{

				case ENET_EVENT_TYPE_CONNECT:
				{
					size_t id = id_generator.GetId();

					event.peer->data = reinterpret_cast<void*>(id);//we can make this point to a structure fo the players data

					if (id < max_players)
					{
						peers[id] = event.peer;
						connected_peers.insert(event.peer);
						//Handle connect
						Handle(event.peer, PeerConnected(id));
					}
					else
					{
						//Server full
						//Shouldn't happen because ENet already has max connections limited but ok, maybe we might switch sometime so lets leave this
					}
				}
				break;

				case ENET_EVENT_TYPE_RECEIVE:
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

						switch (unique_class_id)
						{
							IMPLEMENT_CASE_FOR(ChatMessage);
						}

						#undef IMPLEMENT_CASE_FOR
					}
					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);
				}
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				{
					size_t id = reinterpret_cast<size_t>(event.peer->data);
					if (id < max_players)
					{
						//Handle disconnect
						Handle(event.peer, PeerDisconnected(id));

						peers[id] = nullptr;
						connected_peers.erase(event.peer);
						id_generator.FreeId(id);
					}
				}
				break;

			case ENET_EVENT_TYPE_NONE:
				//plz no warnings
				break;

			}
		}

		HandleTick();
	}

	~Server()
	{

	}
};

std::unique_ptr<Server> server;
int main()
{
	ChatMessage object;
	object.SetMessage(L"hello world");

	/*std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
	size_t unique_id = object.Event_Id();
	ss.write(reinterpret_cast<char*>(&unique_id), sizeof(size_t));
	cereal::BinaryOutputArchive oarchive(ss);
	oarchive(object);
	std::vector<char> vec(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>());*/

	std::cout << "Plz add some light" << std::endl;
	server = std::make_unique<Server>("0.0.0.0", 5544, 32);
	while (true)
	{
		server->Tick();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	return 0;
}
