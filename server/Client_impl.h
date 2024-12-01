#pragma once
#include "Client.h"

template<typename T>
inline void Client::Send(NetworkMessage::ServerPackets packetid, const T* message)
{
	flatbuffers::FlatBufferBuilder builder;

	auto serialized_message = T::Pack(builder, message);
	builder.Finished(serialized_message);

	auto packet_wrapper = NetworkMessage::CreatePacketWrapper(builder,
		static_cast<uint16_t>(packetid),
		builder.CreateVector(builder.GetBufferPointer(), builder.GetSize()));
	builder.Finished(packet_wrapper);

	uint8_t* buf = builder.GetBufferPointer();
	int size = builder.GetSize();

	_send(buf, size);
	
}