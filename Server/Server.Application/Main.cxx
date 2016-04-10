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
#include <IdCounter.hxx>

struct __Player // temporary
{
	ENetPeer* peer;
	size_t id;
	std::wstring name;
	bool spawned;
	uint32_t model_hash;
	Vector3 position;
	Vector3 rotation;

	__Player(ENetPeer* _peer, size_t _id, std::wstring _name)
	{
		peer = _peer;
		id = _id;
		name = _name;
		spawned = false;
	}
};

struct __PlayerPool // temporary
{
	std::list<__Player> players;
} player_pool;

class Server: public MessageReceiver
{
private:
	V_Plus_NetworkServer connection;
	IdCounter id_generator;
	const size_t max_players;
	std::vector<ENetPeer*> peers;
	std::set<ENetPeer*> connected_peers;

	void HandleTick()
	{

	}

	void Handle(const ENetPeer* peer, PeerConnected& data) override
	{
		in_addr x;
		x.S_un.S_addr = peer->address.host;
		std::cout << "Peer connected: " << inet_ntoa(x) << ":" << peer->address.port << " with ID: " << reinterpret_cast<size_t>(peer->data) << std::endl;
	}

	void Handle(const ENetPeer* peer, PeerDisconnected& data) override
	{
		for(auto iter = player_pool.players.begin(); iter != player_pool.players.end();)
		{
			auto temp_iter = iter++;

			if(temp_iter->id == reinterpret_cast<size_t>(peer->data))
			{
				PlayerQuit player_quit;
				player_quit.SetSender(temp_iter->id);

				for(auto &plyr : player_pool.players)
				{
					if(plyr.id != temp_iter->id)
					{
						connection.Send(plyr.peer, player_quit);
					}
				}

				player_pool.players.erase(temp_iter);
			}
		}

		in_addr x;
		x.S_un.S_addr = peer->address.host;
		std::cout << "Peer disconnected: " << inet_ntoa(x) << ":" << peer->address.port << " with ID: " << reinterpret_cast<size_t>(peer->data) << std::endl;
	}

	void Handle(const ENetPeer* peer, ChatMessage& message) override
	{
		message.SetSender(reinterpret_cast<size_t>(peer->data));

		std::wcout << "[" << message.GetSender() << "]: " << message.GetContents() << std::endl;

		connection.Broadcast(message);
	}

	void Handle(const ENetPeer* peer, PlayerJoin& message) override
	{
		message.SetSender(reinterpret_cast<size_t>(peer->data));

		__Player plyr((ENetPeer*)peer, message.GetSender(), message.GetName());
		player_pool.players.push_back(plyr);

		for(auto &plyr : player_pool.players)
		{
			if(plyr.id != message.GetSender())
			{
				PlayerJoin player_join;
				player_join.SetSender(plyr.id);
				player_join.SetName(plyr.name);
				connection.Send((ENetPeer*)peer, player_join);

				if(plyr.spawned)
				{
					PlayerSpawn player_spawn;
					player_spawn.SetSender(plyr.id);
					player_spawn.SetModelHash(plyr.model_hash);
					player_spawn.SetPosition(plyr.position);
					player_spawn.SetRotation(plyr.rotation);
					connection.Send((ENetPeer*)peer, player_spawn);
				}
			}
		}

		std::wcout << "Player joined: " << message.GetName() << " with ID:" << message.GetSender() << std::endl;

		for(auto &plyr : player_pool.players)
		{
			if(plyr.id != message.GetSender())
			{
				connection.Send(plyr.peer, message);
			}
		}
	}

	void Handle(const ENetPeer* peer, PlayerQuit& message) override
	{
		message.SetSender(reinterpret_cast<size_t>(peer->data));

		for(auto iter = player_pool.players.begin(); iter != player_pool.players.end();)
		{
			auto temp_iter = iter++;

			if(temp_iter->id == message.GetSender())
			{
				player_pool.players.erase(temp_iter);
			}
		}

		for(auto &plyr : player_pool.players)
		{
			if(plyr.id != message.GetSender())
			{
				connection.Send(plyr.peer, message);
			}
		}
	}

	void Handle(const ENetPeer* peer, PlayerSpawn& message) override
	{
		message.SetSender(reinterpret_cast<size_t>(peer->data));

		for(auto &plyr : player_pool.players)
		{
			if(plyr.id == message.GetSender())
			{
				Vector3
					_position,
					_rotation;

				plyr.model_hash = message.GetModelHash();

				message.GetPosition(_position);
				plyr.position = _position;

				message.GetRotation(_rotation);
				plyr.rotation = _rotation;

				plyr.spawned = true;
			}
		}

		for(auto &plyr : player_pool.players)
		{
			if(plyr.id != message.GetSender())
			{
				connection.Send(plyr.peer, message);
			}
		}
	}

	void Handle(const ENetPeer* peer, PlayerDespawn& message) override
	{
		message.SetSender(reinterpret_cast<size_t>(peer->data));

		for(auto &plyr : player_pool.players)
		{
			if(plyr.id != message.GetSender())
			{
				connection.Send(plyr.peer, message);
			}
		}
	}

	void Handle(const ENetPeer* peer, OnFootSync& message) override
	{
		message.SetSender(reinterpret_cast<size_t>(peer->data));

		for(auto &plyr : player_pool.players)
		{
			if(plyr.id == message.GetSender())
			{
				Vector3
					_position,
					_rotation;

				message.GetPosition(_position);
				plyr.position = _position;

				message.GetRotation(_rotation);
				plyr.rotation = _rotation;
			}
		}

		for(auto &plyr : player_pool.players)
		{
			if(plyr.id != message.GetSender())
			{
				connection.Send(plyr.peer, message);
			}
		}
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
					ProcessPacketReceiveEvent(event);
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
	std::cout << "Plz add some light" << std::endl;
	server = std::make_unique<Server>("0.0.0.0", 5544, 32);
	while (true)
	{
		server->Tick();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	return 0;
}
