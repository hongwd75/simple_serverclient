#include "EntityManager.h"

void EntityManager::ConnectToServer()
{
	client.connect("ws://localhost:1000");
}

void EntityManager::CloseSocket()
{
	AI.Sleep();
	client.close();
}

void EntityManager::OnWorldEnterNotify(int zoneid)
{
	AI.Wakeup();
}
