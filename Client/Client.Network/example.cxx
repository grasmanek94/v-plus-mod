#include <enet/enetpp.hxx>
#include <vector>
#include <exception>

enum RPC_List
{
	RPC_PlayerMessage
};

namespace V_Plus
{
	namespace Client
	{
		namespace Network
		{
			class IExchangeable
			{
			public:
				virtual std::vector<char> Serialize() = 0;
				virtual bool Deserialize(const std::vector<char>& serialized_data) = 0;
			};

			class IRPC
			{
			public:
				virtual size_t RPC_Id() const = 0;
				virtual void Call(const std::vector<char>& serialized_data) = 0;
			};

			class Instance
			{
			private:
				NetworkClient client;
				std::vector<IRPC> rpc;
			public:
				Instance()
				{
					int init_code = client.GetInitCode();

					if (init_code)
					{
						// TODO custom exception class
						throw std::exception(("Cannot initialise ENET, error code: " + std::to_string(init_code)).c_str());
					}
					if (!client.Create() || !client.Good())
					{
						// TODO custom exception class
						throw std::exception("ENET host member creation failed");
					}

					//ready to use Connect
				}

				~Instance()
				{

				}
			};
		};
	};
};

using namespace V_Plus::Client::Network;

class PlayerMessage : public IExchangeable
{

public:
	std::vector<char> Serialize()
	{

	}

	bool Deserialize(const std::vector<char>& serialized_data)
	{

	}
};

class PlayerMessageRPC : public IRPC
{
	PlayerMessage message;
public:
	size_t RPC_Id()
	{
		return RPC_PlayerMessage;
	}

	void Call(const std::vector<char>& serialized_data)
	{
		if (message.Deserialize(serialized_data))
		{

		}
	}
};