// ** SC_AccountInfo 패킷 메시지
using System.Threading.Tasks;
using Google.FlatBuffers;
using NetworkMessage;

namespace Network.Protocol
{
	[PacketMessageAttribute(ServerPackets.SC_AccountInfo)]
	public class SC_AccountInfo_handler : IPacketMessage
	{
		#pragma warning disable CS1998
		public async Task Packet(ByteBuffer byteBuffer)
		#pragma warning restore CS1998
		{
			SC_AccountInfo_FBS packet = SC_AccountInfo.GetRootAsSC_AccountInfo(byteBuffer).UnPack();
			//Todo 코드 작업 필요
		}
	}
}
