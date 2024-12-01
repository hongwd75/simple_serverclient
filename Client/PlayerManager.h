#pragma once
#include<unordered_map>
#include<mutex>

#include"Player.h"

class PlayerManager
{
public:
	void SetMySession(int se);
	void Add(int session, Player* player);
	void Remove(int session);
	void UpdatePlayerPosition(int session, Vector3 pos, int heading);

	void Lock() { session_mutex.lock(); }
	void UnLock() { session_mutex.unlock(); }
	
public:
	Player* getPlayer(int session);
	Player* getMyPlayer();

private:
	std::mutex session_mutex;
	std::unordered_map<int, Player*> Players;
	int mySessionID = 0;
};

