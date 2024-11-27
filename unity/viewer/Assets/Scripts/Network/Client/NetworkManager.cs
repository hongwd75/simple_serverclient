using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using Google.FlatBuffers;
using NetworkMessage;
using Utility;

namespace Network.Client
{
    public class NetworkManager : MonoSingleton<NetworkManager>
    {
        private NetworkClient _networkClient;
        private PacketProtocolHanlder _protocolHanlder = new PacketProtocolHanlder();

        private readonly Dictionary<ClientPackets, (MethodInfo method, object obj)> clientPackMethods =
            new Dictionary<ClientPackets, (MethodInfo method, object obj)>();
        
        protected override bool dontDistroyFlag
        {
            get => true;
        }

        // 초기화
        protected override void InitClass()
        {
            _networkClient = new WebSocketClient();
            _networkClient.SetCallbackFunctions(OnRecievePacket, null);
        }

        public void SendPacket<T>(ClientPackets id,T message) // where T : class
        {
            var builder = new FlatBufferBuilder(1024);
            var packFunc = GetClientPacketType(id, message);
            
            object packedOffset = packFunc.method.Invoke(packFunc.obj, new object[] { builder, message });
            // builder 완성
            builder.Finish((int)packedOffset.GetType().GetField("Value").GetValue(packedOffset));

            // PacketWrapper 생성 및 전송
            var packetOffset = PacketWrapper.CreatePacketWrapper(
                builder, 
                (ushort)id, 
                PacketWrapper.CreatePacketVector(builder, builder.SizedByteArray())
            );
    
            builder.Finish(packetOffset.Value);
    
            _networkClient.EnqueueSend(new ByteBuffer(builder.SizedByteArray()));            
        }
        
        /// <summary>
        ///  서버 접속 함수
        /// </summary>
        public async Task<bool> ConnectWithRetryAsync(string address, int port, Action success,Action<int> fail,int maxRetries = 3)
        {
            int attempts = 0;
            while (attempts < maxRetries)
            {
                if (await _networkClient.ConnectAsync(address, port))
                {
                    UnityMainThreadDispatcher.Instance.Enqueue(success);
                    return true; // 성공적으로 연결
                }
                
                attempts++;
                UnityMainThreadDispatcher.Instance.Enqueue(()=>fail?.Invoke(attempts));
                await Task.Delay(2000); 
            }
            return false; // 모든 시도 실패
        }


        protected void Update()
        {
            if (_networkClient != null && _networkClient.IsConnected == true)
            {
                _networkClient.ProcessReceivedMessages();
            }
        }

        protected void OnRecievePacket(ByteBuffer packeddata)
        {
            var packet = PacketWrapper.GetRootAsPacketWrapper(packeddata).UnPack();
            _protocolHanlder.OnPacketProcess((ServerPackets)packet.Type,new ByteBuffer(packet.Packet.ToArray()));
        }

        protected (MethodInfo method, object obj) GetClientPacketType<T>(ClientPackets id,T packet)  //where T : class
        {
            if (clientPackMethods.TryGetValue(id, out var packFunc))
            {
                return packFunc;
            }
            else
            {
                string name = packet.ToString();
                if (name.EndsWith("_FBS"))
                {
                    Type type = Type.GetType(name.Substring(0, name.Length - 4));
                    var packMethod = type.GetMethod("Pack", BindingFlags.Public | BindingFlags.Static);
                    if (packMethod != null)
                    {
                        object obj = Activator.CreateInstance(type);

                        clientPackMethods[id] = (packMethod, obj);
                        return clientPackMethods[id];
                    }
                }                
                
            }

            throw new ArgumentException($"지원되지 않는 메시지 타입입니다: {typeof(T).Name}");
        }
    }
}