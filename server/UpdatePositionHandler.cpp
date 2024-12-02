#include "UpdatePositionHandler.h"
#include "ServerMain.h"
#include "FlatBufferUtil.h"
#include <chrono>

void UpdatePositionHandler::HandlePacket(Client* user, const::flatbuffers::Vector<uint8_t>* recvdata)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto packet = flatbuffers::GetRoot<NetworkMessage::CS_UpdatePosition>(recvdata->data());
	user->GetAccount()->UpdatePosition(packet->head(), packet->position()->x(), packet->position()->y(), packet->position()->z());

	// 나를 제외한 나머지에게 브로드 케스팅
	auto connectSessions = ServerMain::instance()->SocketMan()->GetClientManager();

	auto plist = connectSessions->GetRangeUsers(user->GetAccount()->position, 300);
	
	NetworkMessage::SC_UpdatePosition_FBS req;

	req.sessionid = user->GetSessionID();
	req.head = user->GetAccount()->heading;
	req.position = Vector3Convert(user->GetAccount()->position);

	auto sendpacket = FlatBufferUtil::MakeProtocal(NetworkMessage::ServerPackets::ServerPackets_SC_UpdatePosition, &req);
	for (auto player : plist)
	{
		if (player != user)
		{
			player->Send(sendpacket);
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "함수 실행 시간: " << duration.count() << " 마이크로초" << std::endl;
}
