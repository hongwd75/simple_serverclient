#pragma once
#include "..\common\packets\networkmessages_generated.h"

class FlatBufferUtil
{
public:
	template<typename T>
	static std::shared_ptr<std::vector<uint8_t>> MakeProtocal(NetworkMessage::ServerPackets packetid, const T* message);
};

template<typename T>
inline std::shared_ptr<std::vector<uint8_t>> FlatBufferUtil::MakeProtocal(NetworkMessage::ServerPackets packetid, const T* message) {
    // 첫 번째 메시지 직렬화
    flatbuffers::FlatBufferBuilder builder1;
    auto serialized_message = T::TableType::Pack(builder1, message);
    builder1.Finish(serialized_message);

    // 첫 번째 직렬화 데이터를 그대로 참조
    auto packet_data = builder1.GetBufferPointer();
    size_t packet_size = builder1.GetSize();

    // PacketWrapper_FBS 생성
    NetworkMessage::PacketWrapper_FBS pack;
    pack.type = packetid;
    pack.packet = std::vector<uint8_t>(packet_data, packet_data + packet_size); // 복사 최소화

    // 두 번째 메시지 직렬화
    flatbuffers::FlatBufferBuilder builder2;
    auto serialized_message2 = NetworkMessage::PacketWrapper::Pack(builder2, &pack);
    builder2.Finish(serialized_message2);

    // 최종 버퍼 데이터를 복사하여 반환
    return std::make_shared<std::vector<uint8_t>>(builder2.GetBufferPointer(), builder2.GetBufferPointer() + builder2.GetSize());
}
