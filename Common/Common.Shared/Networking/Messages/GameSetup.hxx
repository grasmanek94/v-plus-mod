#pragma once
#include <Networking/SendableEventMessage.hxx>

class GameSetup
{
private:
	size_t player_id;

public:
	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(player_id);
	}

	UniqueClassId_Declare(GameSetup);
};
