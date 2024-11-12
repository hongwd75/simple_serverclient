#include "PlayerManager.h"

void PlayerManager::SetMySession(int se)
{
	mySessionID = se;
}

void PlayerManager::Add(int session, Player* player)
{
	auto itr = Players.find(session);

	if (itr != Players.end())
	{
		delete (*itr).second;
		Players[session] = player;
	}
	else
	{
		Players.insert({ session,player });
	}
}

void PlayerManager::Remove(int session)
{
	auto itr = Players.find(session);

	if (itr != Players.end())
	{
		delete (*itr).second;
		Players.erase(itr);
	}
}

Player* PlayerManager::getPlayer(int session)
{
	auto itr = Players.find(session);

	if (itr != Players.end())
	{
		return (*itr).second;
	}
	return nullptr;
}

Player* PlayerManager::getMyPlayer()
{
	return getPlayer(mySessionID);
}

void PlayerManager::UpdatePlayerPosition(int session, Vector3 pos,int heading)
{
	std::lock_guard<std::mutex> lock(session_mutex);
	auto player = getPlayer(session);
	if (player != nullptr)
	{
		player->UpdatePosition(heading, pos);
	}
}
