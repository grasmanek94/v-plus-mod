#pragma once
#include <enet/enetpp.hxx>

class EventConnect
{
public:
	ENetPeer* peer;
	EventConnect(ENetPeer* peer);
};
