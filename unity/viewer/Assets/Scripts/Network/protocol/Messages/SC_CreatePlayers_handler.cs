// ** SC_CreatePlayers 패킷 메시지

using System.Threading.Tasks;
using Google.FlatBuffers;
using Logic.SceneMain;
using NetworkMessage;
using Players.Players;
using Players.Structs;

namespace Network.Protocol
{
	[PacketMessageAttribute(ServerPackets.SC_CreatePlayers)]
	public class SC_CreatePlayers_handler : IPacketMessage
	{
		#pragma warning disable CS1998
		public async Task Packet(ByteBuffer byteBuffer)
		#pragma warning restore CS1998
		{
			SC_CreatePlayers_FBS packet = SC_CreatePlayers.GetRootAsSC_CreatePlayers(byteBuffer).UnPack();

			baseObjectData dummy = new baseObjectData();
			foreach (var playerdata in packet.Player)
			{
				GamePlayer player = new GamePlayer();
				UnityEngine.Vector3 pos = new UnityEngine.Vector3(playerdata.Position.X, playerdata.Position.Y, playerdata.Position.Z);
				
				player.Create(playerdata.Sessionid,playerdata.Name,dummy,pos,playerdata.Head,playerdata.Realm);
			
				EntityManager.Instance.zoneObjectManager.AddObject(playerdata.Sessionid,player);				
			}
			
		}
	}
}
