#include "WorldJoinReqHandler.h"
#include "ServerMain.h"

void WorldJoinReqHandler::HandlePacket(Client* user, const::flatbuffers::Vector<uint8_t>* recvdata)
{
	auto packet = flatbuffers::GetRoot<NetworkMessage::CS_WorldJoinReq>(recvdata->data());
	if (packet != nullptr)
	{
		// zone 정보 전송
		NetworkMessage::SC_WorldEnterNotify_FBS enterinfo;
		enterinfo.zoneid = 1;
		user->Send(NetworkMessage::ServerPackets::ServerPackets_SC_WorldEnterNotify, &enterinfo);

		bool reconnect = false;
		// 다른 사용자에게 알림
		if (packet->gate() == 0)
		{
			// 이미 접속되어 있었음
			if (user->GetPlayerState() != Enums::ClientState::Room)
			{
				user->GetAccount()->UpdatePosition(rand() % 359, rand() % 300, 1, rand() % 300);
			}
			else
			{
				reconnect = true;
			}
		}

		// 기존 자리를 찾아서 넘겨줘야 하지만 지금은 랜덤으로 1~ 300 사이의 값을 전달하자.
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

		auto sendpacket = FlatBufferUtil::MakeProtocal(NetworkMessage::ServerPackets::ServerPackets_SC_CreatePlayer, &req);

		// 범위를 정하는 코드가 있어야 하지만 우선은 모두에게 브로드케스팅하자
		user->SetPlayerState(Enums::ClientState::Room);

		auto list = connectSessions->GetRangeUsers(user->GetAccount()->position, 1000);
		auto sendlist = connectSessions->GetConnectVector(list, user);

		ServerMain::instance()->SocketMan()->SendBroadcast(sendlist, sendpacket);

		// 이미 있는 플레이어 정보 전송
		for (auto snd : list)
		{
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
