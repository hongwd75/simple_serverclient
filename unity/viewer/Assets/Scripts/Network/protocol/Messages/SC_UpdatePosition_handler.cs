// ** SC_UpdatePosition 패킷 메시지
using System.Threading.Tasks;
using Google.FlatBuffers;
using Logic.SceneMain;
using NetworkMessage;

namespace Network.Protocol
{
	[PacketMessageAttribute(ServerPackets.SC_UpdatePosition)]
	public class SC_UpdatePosition_handler : IPacketMessage
	{
		#pragma warning disable CS1998
		public async Task Packet(ByteBuffer byteBuffer)
		#pragma warning restore CS1998
		{
			SC_UpdatePosition_FBS packet = SC_UpdatePosition.GetRootAsSC_UpdatePosition(byteBuffer).UnPack();
			
			EntityManager.Instance.zoneObjectManager.ModifyObject(packet.Sessionid,(o =>
			{
				UnityEngine.Vector3 pos = new UnityEngine.Vector3(packet.Position.X, packet.Position.Y, packet.Position.Z);
				o.OnMoveTo(pos, 1, packet.Head);
			}));
		}
	}
}
