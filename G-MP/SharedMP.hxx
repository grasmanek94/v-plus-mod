#pragma once

#include <iostream>
#include <enet/enetpp.hxx>
#include "script.hxx"

class CMultiplayer
{
	NetworkBase		* base;
	NetworkServer	* server;
	NetworkClient	* client;

	void DisableAI()
	{
		PED::SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
		PED::SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(0.0, 0.0);

		VEHICLE::SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
		VEHICLE::SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
		VEHICLE::SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);

		PLAYER::SET_MAX_WANTED_LEVEL(0);
	}

public:

	CMultiplayer(void)
	{
		base = nullptr;
		server = nullptr;
		client = nullptr;
	}
	
	bool Send(const void* data, size_t bytes, _ENetPacketFlag flags)
	{
		if (base == nullptr)
		{
			return false;
		}

		if (server != nullptr)
		{
			server->Broadcast(data, bytes, flags);
		}
		else
		{
			client->Send(data, bytes, flags);
		}

		return true;
	}

	bool Start(bool asServer, std::string host, unsigned short port)
	{
		if (base != nullptr)
		{
			return false;
		}

		if (asServer)
		{
			server = new NetworkServer();
			server->SetHost(host, port);

			if (server->GetInitCode()
				|| !server->Create(1024)
				|| !server->Good())
			{
				delete server;
				std::cout << "An error occurred while trying to create an ENet server." << std::endl;
				return false;
			}

			base = server;
		}
		else
		{
			client = new NetworkClient();

			if (client->GetInitCode()
				|| !client->Create()
				|| !client->Good()
				|| !client->Connect(host, port))
			{
				delete client;
				std::cout << "An error occurred while trying to create an ENet client." << std::endl;
				return false;
			}

			base = client;
		}

		return true;
	}

	void Process(void)
	{
		if (base != nullptr)
		{
			//if (server == nullptr)
			{
				DisableAI();
			}

			if (base->Pull())
			{
				ENetEvent event = base->Event();
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					printf("A new connection has been made with %x:%u.\n",
						event.peer->address.host,
						event.peer->address.port);
					/* Store any relevant client information here. */
					event.peer->data = "info";
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					//printf("%s\n", event.packet->data);

					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);

					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("%x disconnected.\n", event.peer->address.host);
					/* Reset the peer's client information. */
					event.peer->data = NULL;
					if (client != nullptr)
					{
						client->ReConnect();
					}
					break;
				}
			}
		}
	}
};
