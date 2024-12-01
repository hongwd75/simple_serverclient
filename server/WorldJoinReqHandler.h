#pragma once
#include "PacketHandler.h"

class WorldJoinReqHandler : public  PacketHandler
{
public:
	void HandlePacket(Client* user, const ::flatbuffers::Vector<uint8_t>* recvdata) override;
};

