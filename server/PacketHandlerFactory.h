#pragma once
#include <functional>
#include <memory>
#include "..\common\packets\networkmessages_generated.h"
#include "PacketHandler.h"
class PacketHandlerFactory
{
public:
    PacketHandlerFactory();
public:
    static PacketHandlerFactory& Instance()
    {
        static PacketHandlerFactory instance;
        return instance;
    }

    void RegisterHandler(NetworkMessage::ClientPackets id, std::function<std::unique_ptr<PacketHandler>()> creator)
    {
        m_handlers[id] = creator;
    }

    std::unique_ptr<PacketHandler> GetHandler(NetworkMessage::ClientPackets id)
    {
        auto it = m_handlers.find(id);
        if (it != m_handlers.end())
        {
            return it->second();
        }
        return nullptr;
    }




private:
    std::map<NetworkMessage::ClientPackets, std::function<std::unique_ptr<PacketHandler>()>> m_handlers;
};

