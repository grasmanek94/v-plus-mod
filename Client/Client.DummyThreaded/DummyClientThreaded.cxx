#include <exception>
#include <thread>
#include <Networking/Networking.hxx>
#include <SharedUtility.h>
#include <string>
#include <sstream>
#include <concurrent_queue.h>
#include <atomic>

class ExtremelyThreadedClient : public V_Plus_NetworkClient
{
private:
	Concurrency::concurrent_queue<std::string> streams;
	Concurrency::concurrent_queue<std::wstring> wstreams;
	std::atomic<bool> connected;

	void EventConnectReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<EventConnect> data(V_Plus_NetworkClient::EventConnect_handler.TryGet());

			if (data)
			{
				connected = true;

				in_addr x;
				x.S_un.S_addr = data->peer->address.host;

				std::stringstream s;
				s << "EventConnect: " << inet_ntoa(x) << ":" << data->peer->address.port << " with ID: " << reinterpret_cast<size_t>(data->peer->data) << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (EventConnect_handler.GetCount() + 1)));
		}
	}

	void EventDisconnectReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<EventDisconnect> data(V_Plus_NetworkClient::EventDisconnect_handler.TryGet());

			if (data)
			{
				connected = false;

				in_addr x;
				x.S_un.S_addr = data->peer->address.host;

				std::stringstream s;
				std::cout << "EventDisconnect: " << inet_ntoa(x) << ":" << data->peer->address.port << " with ID: " << reinterpret_cast<size_t>(data->peer->data) << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (EventDisconnect_handler.GetCount() + 1)));
		}
	}

	void PeerConnectedReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<PeerConnected> data(V_Plus_NetworkClient::PeerConnected_handler.TryGet());

			if (data)
			{
				std::stringstream s;
				s << "Peer connected with ID: " << data->Id() << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (PeerConnected_handler.GetCount() + 1)));
		}
	}

	void PeerDisconnectedReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<PeerDisconnected> data(V_Plus_NetworkClient::PeerDisconnected_handler.TryGet());

			if (data)
			{
				std::stringstream s;
				s << "Peer disconnected with ID: " << data->Id() << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (PeerDisconnected_handler.GetCount() + 1)));
		}
	}

	void ChatMessageReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<ChatMessage> data(V_Plus_NetworkClient::ChatMessage_handler.TryGet());

			if (data)
			{
				std::wstringstream s;
				s << "Chatmsg from [" << data->GetSender() << "]: " << data->GetContents() << std::endl;
				wstreams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (ChatMessage_handler.GetCount() + 1)));
		}
	}

	void PlayerJoinReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<PlayerJoin> data(V_Plus_NetworkClient::PlayerJoin_handler.TryGet());

			if (data)
			{
				std::stringstream s;
				s << "Player joined with ID: " << data->GetSender() << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (PlayerJoin_handler.GetCount() + 1)));
		}
	}

	void PlayerQuitReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<PlayerQuit> data(V_Plus_NetworkClient::PlayerQuit_handler.TryGet());

			if (data)
			{
				std::stringstream s;
				s << "Player quit with ID: " << data->GetSender() << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (PlayerQuit_handler.GetCount() + 1)));
		}
	}

	void PlayerSpawnReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<PlayerSpawn> data(V_Plus_NetworkClient::PlayerSpawn_handler.TryGet());

			if (data)
			{
				std::stringstream s;
				s << "Player spawned with ID: " << data->GetSender() << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (PlayerSpawn_handler.GetCount() + 1)));
		}
	}

	void PlayerDespawnReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<PlayerDespawn> data(V_Plus_NetworkClient::PlayerDespawn_handler.TryGet());

			if (data)
			{
				std::stringstream s;
				s << "Player despawned with ID: " << data->GetSender() << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (PlayerDespawn_handler.GetCount() + 1)));
		}
	}

	void OnFootSyncReceiveThread()
	{
		while (true)
		{
			std::shared_ptr<OnFootSync> data(V_Plus_NetworkClient::OnFootSync_handler.TryGet());

			if (data)
			{
				std::stringstream s;
				Vector3 pos;
				data->GetPosition(pos);
				s << "Player OnFootsync with ID: " << data->GetSender() << " POS:[" << pos.x << "; " << pos.y << "; " << pos.z << "]" << std::endl;
				streams.push(s.str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50 / (OnFootSync_handler.GetCount() + 1)));
		}
	}

	void OtherReceiveThread()
	{
		while (true)
		{
			V_Plus_NetworkClient::WorldUpdate_handler.TryGet();
			V_Plus_NetworkClient::GameSetup_handler.TryGet();

			std::this_thread::sleep_for(std::chrono::milliseconds(100 / (GameSetup_handler.GetCount() + WorldUpdate_handler.GetCount() + 1)));
		}
	}

	void NetworkThread()
	{
		while (true)
		{
			V_Plus_NetworkClient::RunAsync();
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}

	void OnFootSyncSenderThread()
	{
		Vector3 pos(0.0f, 0.0f, 0.0f);
		OnFootSync sync;
		while (true)
		{
			pos.x += 1.0f;
			pos.y -= 1.0f;
			pos.z = 2.0f * pos.x - pos.y;
			sync.SetPosition(pos);

			std::stringstream s;
			s << "Sending OnFoot Sync: POS:[" << pos.x << "; " << pos.y << "; " << pos.z << "]" << std::endl;
			streams.push(s.str());

			V_Plus_NetworkClient::SendAsync(sync);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

	void ChatMessageSenderThread()
	{
		while (true)
		{
			ChatMessage message;
			message.SetContents(L"Hello there, here is a Dummy Client Threaded!");
			V_Plus_NetworkClient::SendAsync(message);
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		}
	}

	void ConsoleTextSyncThread()
	{
		while (true)
		{
			std::string stream;
			if (streams.try_pop(stream))
			{
				std::cout << stream << std::flush;
			}
			std::wstring wstream;
			if (wstreams.try_pop(wstream))
			{
				std::wcout << wstream << std::flush;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}
/////////////////////////


	std::vector<std::thread> threads;

public:
	ExtremelyThreadedClient()
		: connected(false)
	{
		int init_code = V_Plus_NetworkClient::GetInitCode();

		if (init_code)
		{
			// TODO custom exception class
			throw std::exception(("Cannot initialize ENET, error code: " + std::to_string(init_code)).c_str());
		}

		if (!V_Plus_NetworkClient::Create() || !V_Plus_NetworkClient::Good())
		{
			// TODO custom exception class
			throw std::exception("ENET host member creation failed");
		}

		//ready to use Connect
	}

	void Connect(const std::string& host, uint16_t port, const std::string& password)
	{
		V_Plus_NetworkClient::Connect(host, port);
	}

	void Run()
	{
		threads.push_back(std::thread(&ExtremelyThreadedClient::EventConnectReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::EventDisconnectReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::PeerConnectedReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::PeerDisconnectedReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::ChatMessageReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::PlayerJoinReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::PlayerQuitReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::PlayerSpawnReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::PlayerDespawnReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::OnFootSyncReceiveThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::NetworkThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::OnFootSyncSenderThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::ChatMessageSenderThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::ConsoleTextSyncThread, this));
		threads.push_back(std::thread(&ExtremelyThreadedClient::OtherReceiveThread, this));

		for(auto& thread: threads)
		{
			thread.join();
		}
	}

	~ExtremelyThreadedClient()
	{

	}
};

std::unique_ptr<ExtremelyThreadedClient> client;
int main()
{
	std::cout << "Plz add some light threaded" << std::endl;
	client = std::make_unique<ExtremelyThreadedClient>();
	client->Connect("127.0.0.1", 5544, "");
	client->Run();
	return 0;
}
