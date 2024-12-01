// ** SC_CreatePlayer 패킷 메시지
using System.Threading.Tasks;
using Google.FlatBuffers;
using Logic.SceneMain;
using NetworkMessage;
using Players.Players;
using Players.Structs;
using Vector3 = UnityEngine.Vector3;

namespace Network.Protocol
{
	[PacketMessageAttribute(ServerPackets.SC_CreatePlayer)]
	public class SC_CreatePlayer_handler : IPacketMessage
	{
		#pragma warning disable CS1998
		public async Task Packet(ByteBuffer byteBuffer)
		#pragma warning restore CS1998
		{
			SC_CreatePlayer_FBS packet = SC_CreatePlayer.GetRootAsSC_CreatePlayer(byteBuffer).UnPack();

			Vector3 pos = new Vector3(packet.Player.Position.X, packet.Player.Position.Y, packet.Player.Position.Z);
			baseObjectData dummy = new baseObjectData();
			
			GamePlayer player = new GamePlayer();
			player.Create(packet.Player.Sessionid,packet.Player.Name,dummy,pos,packet.Player.Head,packet.Player.Realm);
			
			EntityManager.Instance.zoneObjectManager.AddObject(packet.Player.Sessionid,player);
		}
	}
}
