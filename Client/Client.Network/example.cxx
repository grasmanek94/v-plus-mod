#include <enet/enetpp.hxx>
#include <vector>
#include <exception>
#include <array>
#include <memory>

#include <cereal/archives/binary.hpp>

size_t constexpr const_hash(char const *input) 
{
	return *input ?
		static_cast<size_t>(*input) + 33 * const_hash(input + 1) :
		5381;
}

// when using this interface, it is mandatory to implement 
// private:	
//	template<class Archive> void serialize(Archive& archive)
class IEvent
{
public:
	virtual size_t Event_Id() const = 0;
};

class ChatMessage : public IEvent
{
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(message);
	}

	std::wstring message;
public:

	size_t Event_Id() const
	{
		return const_hash("ChatMessage");
	}

	const std::wstring getMessage() const
	{
		return message;
	}

	void setMessage(const std::wstring& msg)
	{
		message = msg;
	}
};

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