#include "PacketHandler.h"

std::shared_ptr<NetworkMessage::Vector3> PacketHandler::Vector3Convert(Structs::Vector3 pos)
{
	return std::make_shared<NetworkMessage::Vector3>(pos.x,pos.y, pos.z);
}
