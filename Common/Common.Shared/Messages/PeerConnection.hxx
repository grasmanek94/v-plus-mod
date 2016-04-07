#pragma once
#include <cstdlib>

class PeerConnected
{
private:
	size_t id;

public:
	PeerConnected(size_t id);
	size_t Id() const;
};

class PeerDisconnected
{
private:
	size_t id;

public:
	PeerDisconnected(size_t id);

	size_t Id() const;
};