using System;
using NetworkMessage;

namespace Network.Protocol
{
    [AttributeUsage(AttributeTargets.Class,AllowMultiple = false)]
    public class PacketMessageAttribute : Attribute
    {
        public readonly ServerPackets ID;

        public PacketMessageAttribute(ServerPackets id)
        {
            ID = id;
        }
    }
}