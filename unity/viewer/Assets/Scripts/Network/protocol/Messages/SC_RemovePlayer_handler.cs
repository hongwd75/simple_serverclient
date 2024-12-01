// ** SC_RemovePlayer 패킷 메시지
using System.Threading.Tasks;
using Google.FlatBuffers;
using Logic.SceneMain;
using NetworkMessage;

namespace Network.Protocol
{
	[PacketMessageAttribute(ServerPackets.SC_RemovePlayer)]
	public class SC_RemovePlayer_handler : IPacketMessage
	{
		#pragma warning disable CS1998
		public async Task Packet(ByteBuffer byteBuffer)
		#pragma warning restore CS1998
		{
			SC_RemovePlayer_FBS packet = SC_RemovePlayer.GetRootAsSC_RemovePlayer(byteBuffer).UnPack();
			EntityManager.Instance.zoneObjectManager.RemoveObject(packet.Sessionid);
		}
	}
}
