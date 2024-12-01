#pragma once
#include <websocketpp/common/connection_hdl.hpp>
#include "Client.h"
class PacketHandler
{
public:
	virtual ~PacketHandler() = default;
	virtual void HandlePacket(Client* user, const ::flatbuffers::Vector<uint8_t>* recvdata) = 0;

public:
	std::shared_ptr< NetworkMessage::Vector3> Vector3Convert(Structs::Vector3 pos);
};

