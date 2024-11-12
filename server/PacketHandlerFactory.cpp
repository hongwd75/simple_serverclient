#include "PacketHandlerFactory.h"
//--- ��Ŷ ����Ʈ ����
#include "LoginReqHandler.h"
#include "WorldJoinReqHandler.h"
#include "UpdatePositionHandler.h"
//--- ��Ŷ ����Ʈ ����

#define REGISTERPACKET(packetType, handlerClass) \
    RegisterHandler(packetType, []() -> std::unique_ptr<PacketHandler> { \
        return std::make_unique<handlerClass>(); \
    });

PacketHandlerFactory::PacketHandlerFactory()
{
    REGISTERPACKET(NetworkMessage::ClientPackets::ClientPackets_CS_LoginReq, LoginReqHandler);
    REGISTERPACKET(NetworkMessage::ClientPackets::ClientPackets_CS_WorldJoinReq, WorldJoinReqHandler);
    REGISTERPACKET(NetworkMessage::ClientPackets::ClientPackets_CS_UpdatePosition, UpdatePositionHandler);
}
