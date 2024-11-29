using System;
using System.Collections.Generic;
using System.Reflection;
using System.Threading.Tasks;
using Google.FlatBuffers;
using Network.Protocol;
using NetworkMessage;
using UnityEngine;

namespace Network.Client
{
    public class PacketProtocolHanlder
    {
        private Dictionary<ServerPackets, Func<ByteBuffer, Task>> _packetMessages = new();

        // 리스트 작성
        public PacketProtocolHanlder()
        {
            var assembly = Assembly.GetExecutingAssembly();
            foreach (Type type in assembly.GetTypes())
            {
                if(type.IsClass == false) continue;
                if (type.GetInterface("Network.Protocol.IPacketMessage") == null) continue;

                var packetattributes = (PacketMessageAttribute[])type.GetCustomAttributes(typeof(PacketMessageAttribute), true);
                if (packetattributes.Length > 0)
                {
                    var handle = (IPacketMessage)Activator.CreateInstance(type);
                    _packetMessages.Add(packetattributes[0].ID, async (data) => { await handle.Packet(data);});
                }
            }
        }

        public void OnPacketProcess(ServerPackets id, ByteBuffer data)
        {
            if (_packetMessages.TryGetValue(id, out var handler) == true)
            {
                Debug.Log($" 받은 패킷 : {id.ToString()}");
                _= handler.Invoke(data);
            }
        }
    }
}