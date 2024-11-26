using System.Threading.Tasks;
using Google.FlatBuffers;

namespace Network.Protocol
{
    public interface IPacketMessage
    {
        Task Packet(ByteBuffer buteBuffer);
    }
}