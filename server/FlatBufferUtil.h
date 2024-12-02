#pragma once
#include "..\common\packets\networkmessages_generated.h"

class FlatBufferUtil
{
public:
	template<typename T>
	static flatbuffers::FlatBufferBuilder MakeProtocal(NetworkMessage::ServerPackets packetid, const T* message);
};

template<typename T>
inline flatbuffers::FlatBufferBuilder FlatBufferUtil::MakeProtocal(NetworkMessage::ServerPackets packetid, const T* message)
{
	flatbuffers::FlatBufferBuilder builder;
	auto serialized_message = T::TableType::Pack(builder, message);
	builder.Finish(serialized_message);

	std::vector<uint8_t> packet_data(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize());

	NetworkMessage::PacketWrapper_FBS pack;
	pack.type = packetid;
	pack.packet = packet_data;


	flatbuffers::FlatBufferBuilder builder2;
	auto serialized_message2 = NetworkMessage::PacketWrapper::Pack(builder2, &pack);
	builder2.Finish(serialized_message2);
	return builder2;
}
