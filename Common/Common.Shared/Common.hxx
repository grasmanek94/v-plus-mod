#include <cereal/archives/binary.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/valarray.hpp>
#include <cereal/types/vector.hpp>

static_assert(sizeof(size_t) == 8, "You cannot compile V-Plus for the chosen architecture");

const size_t constexpr const_hash(char const *input)
{
	return *input ?
		static_cast<size_t>(*input) + 33 * const_hash(input + 1) :
		5381;
}

#define UniqueClassId_Declare() static const size_t UniqueClassId(); size_t Event_Id() const
#define UniqueClassId_Implement(className) static constexpr size_t className::UniqueClassId() { return const_hash(#className); } const size_t className::Event_Id() const { return UniqueClassId(); }
#define UniqueClassId_ImplementInline(className) static constexpr size_t UniqueClassId() { return const_hash(#className); } const size_t Event_Id() const { return UniqueClassId(); }

class ChatMessage
{
private:
	std::wstring message;

public:
	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(message);
	}
	UniqueClassId_ImplementInline(ChatMessage);

	ChatMessage()
	{ }

	const std::wstring GetMessage() const
	{
		return message;
	}

	void SetMessage(const std::wstring& msg)
	{
		message = msg;
	}
};

class PeerConnected
{
private:
	size_t id;

public:
	PeerConnected(size_t id)
		: id(id)
	{ }

	size_t Id() const
	{
		return id;
	}
};

class PeerDisconnected
{
private:
	size_t id;

public:
	PeerDisconnected(size_t id)
		: id(id)
	{ }

	size_t Id() const
	{
		return id;
	}
};