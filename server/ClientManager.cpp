#include "ClientManager.h"
#include "ConsoleDisplay.h"
#include "WebSocketServer.h"

ClientManager::ClientManager(WebSocketServer* _server): server(_server), ClientSlot(MaxUserSize+1)
{
	for (int i = 1; i <= MaxUserSize; ++i) {
		emptyClientSlot.push(i);
	}
}

//----------------------------------------------------------------------------------
#pragma region 소켓 세션 관리
bool ClientManager::AddSession(websocketpp::connection_hdl hdl)
{
	if (emptyClientSlot.empty() == false)
	{
		int slot = emptyClientSlot.front();
		ClientSlot[slot] = std::make_unique<Client>(hdl, slot, server);
		sock2Client.insert({ hdl, slot });
		emptyClientSlot.pop();

		// ToDo. 이후 코드 고도화로 접속 대기하는 소켓은 모두 삭제하게 해야 한다.
		// waitloginSession.push_back(hdl);
		return true;
	}
	return false;
}

bool ClientManager::AddLoginedUser(Client *client)
{
	auto itr = Clients.find(client->GetAccount()->UID);
	if (itr != Clients.end())
	{
		return false;
	}
	Clients.insert({ client->GetAccount()->UID, client->GetSessionID() });
	return true;
}

bool ClientManager::ChangeSocketSession(uint64_t uid, websocketpp::connection_hdl newsocket)
{
	auto itr = Clients.find(uid);

	if (itr != Clients.end())
	{
		auto client = ClientSlot[(*itr).second].get();
		if (client != nullptr)
		{
			client->SetSocket(newsocket);

			auto skitr = sock2Client.find(newsocket);
			if (skitr != sock2Client.end())
			{
				ClientSlot[(*skitr).second] = nullptr;
				emptyClientSlot.push((*skitr).second);
				(*skitr).second = client->GetSessionID();
				return true;
			}
		}
	}
	return false;
}

void ClientManager::RemoveSession(websocketpp::connection_hdl hdl)
{
	std::lock_guard<std::mutex> lock(session_mutex);
	auto itr = sock2Client.find(hdl);
	if (itr != sock2Client.end())
	{
		Client* ptr = ClientSlot[(*itr).second].get();
		if (ptr != nullptr)
		{
			if (ptr->GetPlayerState() == Enums::ClientState::Room)
			{
				// 룸에 있는 플레이어는 재접속 후, 플레이가 계속 이어져야 한다.
			}
			else
			{
				auto uid = ptr->GetAccount()->UID;
				if (uid > 0)
				{
					Clients.erase(uid);
				}
				ClientSlot[ptr->GetSessionID()] = nullptr;
				emptyClientSlot.push(ptr->GetSessionID());
			}
			sock2Client.erase(itr);
		}
	}
}

void ClientManager::RemoveSeesionByUUID(uint64_t uid)
{
	if (uid < 1)
	{
		return;
	}
	std::lock_guard<std::mutex> lock(session_mutex);
	auto itr = Clients.find(uid);

	if (itr != Clients.end())
	{
		Client* ptr = ClientSlot[(*itr).second].get();
		if (ptr != nullptr)
		{
			auto uid = ptr->GetAccount()->UID;
			if (uid > 0)
			{
				Clients.erase(uid);
			}
			emptyClientSlot.push(ptr->GetSessionID());
			Clients.erase(itr);
			auto sockitr = sock2Client.find(ptr->GetSocket());
			if (sockitr != sock2Client.end())
			{
				if ((*sockitr).second == ptr->GetSessionID())
				{
					sock2Client.erase(ptr->GetSocket());
				}
			}
			
			ClientSlot[ptr->GetSessionID()] = nullptr;
		}
	}
}

#pragma endregion

//----------------------------------------------------------------------------------
#pragma region 클라이언트 추가/삭제/얻기
std::vector<Client*> ClientManager::GetRangeUsers(Structs::Vector3 pos, int range)
{
	std::vector<Client*> rangeusers;
	for (const auto& client : Clients) {

		int client_data = client.second;  // 클라이언트 데이터 (예: 해당 시간에 관련된 정보)
		auto user = ClientSlot[client_data].get();
		if (user != nullptr)
		{
			if (user->GetPlayerState() == Enums::ClientState::Room)
			{
				rangeusers.push_back(user);
			}
		}
	}
	return rangeusers;
}

Client* ClientManager::GetClientByUID(uint64_t uid)
{
	auto itr = Clients.find(uid);
	if (itr != Clients.end())
	{
		return ClientSlot[(*itr).second].get();
	}

	return nullptr;
}

Client* ClientManager::GetClient(websocketpp::connection_hdl hdl)
{
	auto itr = sock2Client.find(hdl);
	if (itr != sock2Client.end())
	{
		return ClientSlot[(*itr).second].get();
	}
	return nullptr;
}

Client* ClientManager::GetClientBySession(int sessionid)
{
	if (ClientSlot[sessionid] != nullptr)
	{
		return ClientSlot[sessionid].get();
	}
	return nullptr;
}


#pragma endregion