#pragma once
#include <enet/enetpp.hxx>

class EventDisconnect
{
public:
	ENetPeer* peer;
	EventDisconnect(ENetPeer* peer);
};
