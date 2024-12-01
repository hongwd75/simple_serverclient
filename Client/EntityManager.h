#pragma once
#include "PlayerManager.h"
#include "WebSocketClient.h"
#include "AutoPlay.h"

class EntityManager
{
public:
	static EntityManager& instance()
	{
		static EntityManager _instance;
		return _instance;
	}

public:
	void ConnectToServer();
	void CloseSocket();
	void OnWorldEnterNotify(int zoneid);

public:
	std::atomic<bool> zoneLoaded = false;

	PlayerManager players;
	WebSocketClient client;

	std::string accountid = "normal_1";
	std::string password = "1";

	AutoPlay AI;
};

