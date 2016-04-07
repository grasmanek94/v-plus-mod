#include <enet/enetpp.hxx>
#include <vector>
#include <exception>
#include <array>
#include <memory>
#include <Common.hxx>

class Instance
{
private:
	NetworkClient connection;
public:
	Instance()
	{
		ChatMessage x;

		int init_code = connection.GetInitCode();

		if (init_code)
		{
			// TODO custom exception class
			throw std::exception(("Cannot initialize ENET, error code: " + std::to_string(init_code)).c_str());
		}

		if (!connection.Create() || !connection.Good())
		{
			// TODO custom exception class
			throw std::exception("ENET host member creation failed");
		}


		//ready to use Connect
	}

	void Tick()
	{
		if (connection.Pull())
		{
			ENetEvent event = connection.Event();

			switch (event.type)
			{

			case ENET_EVENT_TYPE_CONNECT:
				
				break;

			case ENET_EVENT_TYPE_RECEIVE:

				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				
				break;

			case ENET_EVENT_TYPE_NONE:
				//plz no warnings
				break;

			}
		}
	}

	~Instance()
	{

	}
};

Instance y;