// ** SC_LoginAns 패킷 메시지
using System.Threading.Tasks;
using Google.FlatBuffers;
using Logic.SceneMain;
using Network.Client;
using NetworkMessage;
using Players.Structs;
using UnityEngine;

namespace Network.Protocol
{
	[PacketMessageAttribute(ServerPackets.SC_LoginAns)]
	public class SC_LoginAns_handler : IPacketMessage
	{
		#pragma warning disable CS1998
		public async Task Packet(ByteBuffer byteBuffer)
		#pragma warning restore CS1998
		{
			SC_LoginAns_FBS packet = SC_LoginAns.GetRootAsSC_LoginAns(byteBuffer).UnPack();
			PlayerInfo player = new PlayerInfo()
			{
				name = packet.Nickname,
				sessionID = packet.Sessionid,
				realm = 0
			};
			EntityManager.Instance.myPlayerInfo = player;
			WorldJoinReq();
		}

		protected void WorldJoinReq()
		{
			CS_WorldJoinReq_FBS req = new CS_WorldJoinReq_FBS();
			req.Gate = 0;
			NetworkManager.Instance.SendPacket(ClientPackets.CS_WorldJoinReq,req);			
		}
	}
}
