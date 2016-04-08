#include <exception>
#include <thread>
#include <Networking.hxx>

class Client : public MessageReceiver
{
private:
	V_Plus_NetworkClient connection;

	void HandleTick()
	{
		if (GetAsyncKeyState('X'))
		{
			ChatMessage message;
			message.SetContents(L"Hello there, here is a Dummy Client!");
			connection.Send(message);
		}
	}

	void Handle(const ENetPeer* peer, PeerConnected& data) override
	{
		in_addr x;
		x.S_un.S_addr = peer->address.host;
		std::cout << "Host connected: " << inet_ntoa(x) << ":" << peer->address.port << " with ID: " << reinterpret_cast<size_t>(peer->data) << std::endl;
	}

	void Handle(const ENetPeer* peer, PeerDisconnected& data) override
	{
		in_addr x;
		x.S_un.S_addr = peer->address.host;
		std::cout << "Host disconnected: " << inet_ntoa(x) << ":" << peer->address.port << " with ID: " << reinterpret_cast<size_t>(peer->data) << std::endl;
	}

	void Handle(const ENetPeer* peer, ChatMessage& message) override
	{
		std::wcout << "[" << message.GetSender() << "]: " << message.GetContents() << std::endl;
	}

public:
	Client()
	{
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
				std::cout << "Connected!" << std::endl;
				break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				ProcessPacketReceiveEvent(event);
			}
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << "Disconnected!" << std::endl;
				break;

			case ENET_EVENT_TYPE_NONE:
				//plz no warnings
				break;

			}
		}

		HandleTick();
	}

	void Connect(const std::string& host, uint16_t port, const std::string& password)
	{
		connection.Connect(host, port);
	}

	~Client()
	{

	}
};

std::unique_ptr<Client> client;
int main()
{
	std::cout << "Plz add some light" << std::endl;
	client = std::make_unique<Client>();
	client->Connect("127.0.0.1", 5544, "");
	while (true)
	{
		client->Tick();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	return 0;
}
