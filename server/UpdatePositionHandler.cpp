#include "UpdatePositionHandler.h"
#include "ServerMain.h"

void UpdatePositionHandler::HandlePacket(Client* user, const::flatbuffers::Vector<uint8_t>* recvdata)
{
	auto packet = flatbuffers::GetRoot<NetworkMessage::CS_UpdatePosition>(recvdata->data());
	user->GetAccount()->UpdatePosition(packet->head(), packet->position()->x(), packet->position()->y(), packet->position()->z());

	// ���� ������ ���������� ��ε� �ɽ���
	auto connectSessions = ServerMain::instance()->SocketMan()->GetClientManager();

	auto plist = connectSessions->GetRangeUsers(user->GetAccount()->position, 300);
	
	NetworkMessage::SC_UpdatePosition_FBS req;

	req.sessionid = user->GetSessionID();
	req.head = user->GetAccount()->heading;
	req.position = Vector3Convert(user->GetAccount()->position);

	for (auto player : plist)
	{
		if (player != user)
		{
			player->Send(NetworkMessage::ServerPackets::ServerPackets_SC_UpdatePosition, &req);
		}
	}
}
