// ** SC_WorldEnterNotify 패킷 메시지
using System.Threading.Tasks;
using Google.FlatBuffers;
using Logic.SceneMain;
using NetworkMessage;

namespace Network.Protocol
{
	[PacketMessageAttribute(ServerPackets.SC_WorldEnterNotify)]
	public class SC_WorldEnterNotify_handler : IPacketMessage
	{
		#pragma warning disable CS1998
		public async Task Packet(ByteBuffer byteBuffer)
		#pragma warning restore CS1998
		{
			SC_WorldEnterNotify_FBS packet = SC_WorldEnterNotify.GetRootAsSC_WorldEnterNotify(byteBuffer).UnPack();
			//Todo 코드 작업 필요

			EntityManager.Instance.myPlayerInfo.ZoneID = packet.Zoneid;
		}
	}
}
