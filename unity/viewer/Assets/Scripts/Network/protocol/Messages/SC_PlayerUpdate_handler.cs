// ** SC_PlayerUpdate 패킷 메시지
using System.Threading.Tasks;
using Google.FlatBuffers;
using Logic.SceneMain;
using NetworkMessage;

namespace Network.Protocol
{
	[PacketMessageAttribute(ServerPackets.SC_PlayerUpdate)]
	public class SC_PlayerUpdate_handler : IPacketMessage
	{
		#pragma warning disable CS1998
		public async Task Packet(ByteBuffer byteBuffer)
		#pragma warning restore CS1998
		{
			SC_PlayerUpdate_FBS packet = SC_PlayerUpdate.GetRootAsSC_PlayerUpdate(byteBuffer).UnPack();
		}
	}
}
