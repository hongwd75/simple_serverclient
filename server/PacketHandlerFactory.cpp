#include "PacketHandlerFactory.h"
//--- 패킷 리스트 시작
#include "LoginReqHandler.h"
#include "WorldJoinReqHandler.h"
#include "UpdatePositionHandler.h"
//--- 패킷 리스트 종료

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
