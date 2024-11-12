#include "WorldJoinReqHandler.h"
#include "ServerMain.h"

void WorldJoinReqHandler::HandlePacket(Client* user, const::flatbuffers::Vector<uint8_t>* recvdata)
{
	auto packet = flatbuffers::GetRoot<NetworkMessage::CS_WorldJoinReq>(recvdata->data());
	if (packet != nullptr)
	{
		// zone ���� ����
		NetworkMessage::SC_WorldEnterNotify_FBS enterinfo;
		enterinfo.zoneid = 1;
		user->Send(NetworkMessage::ServerPackets::ServerPackets_SC_WorldEnterNotify, &enterinfo);

		// �ٸ� ����ڿ��� �˸�
		if (packet->gate() == 0)
		{
			user->GetAccount()->UpdatePosition(rand() % 359, rand() % 300, rand() % 300, 1);
		}

		// ���� �ڸ��� ã�Ƽ� �Ѱ���� ������ ������ �������� 1~ 300 ������ ���� ��������.
		NetworkMessage::SC_CreatePlayer_FBS req;
		req.player = std::make_shared< NetworkMessage::CreatePlayerInfo_FBS>();
		req.player->realm = user->GetAccount()->realm;
		req.player->sessionid = user->GetSessionID();
		req.player->name = user->GetAccount()->AccountName;
		req.player->head = user->GetAccount()->heading;
		req.player->position = Vector3Convert(user->GetAccount()->position);


		NetworkMessage::SC_CreatePlayers_FBS fornewplayer;
		auto playerArray = &fornewplayer.player;

		auto connectSessions = ServerMain::instance()->SocketMan()->GetClientManager();

		// ������ ���ϴ� �ڵ尡 �־�� ������ �켱�� ��ο��� ��ε��ɽ�������
		user->SetPlayerState(Enums::ClientState::Room);
		auto list = connectSessions->GetRangeUsers(user->GetAccount()->position, 1000);
		for (auto snd : list)
		{
			if (snd != user)
			{
				snd->Send(NetworkMessage::ServerPackets::ServerPackets_SC_CreatePlayer, &req);
			}

			auto playerobject = std::make_shared<NetworkMessage::CreatePlayerInfo_FBS>();
			playerobject->sessionid = snd->GetSessionID();
			playerobject->head = snd->GetAccount()->heading;
			playerobject->realm = snd->GetAccount()->realm;
			playerobject->name = snd->GetAccount()->AccountName;
			playerobject->position = Vector3Convert(snd->GetAccount()->position);
			playerArray->push_back(playerobject);
		}
		user->Send(NetworkMessage::ServerPackets::ServerPackets_SC_CreatePlayers, &fornewplayer);
		
	}
}
