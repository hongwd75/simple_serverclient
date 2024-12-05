#include "AutoPlay.h"
#include "EntityManager.h"
#include <iostream>
#include <cmath>
#include <random>



float getRandomFloatInRange(float lower, float upper) {
	thread_local static std::random_device rd;
	thread_local static std::mt19937 generator(rd());
	thread_local static std::uniform_real_distribution<float> distribution(lower, upper);
	return distribution(generator);
}


AutoPlay::AutoPlay() : thinking(false), directmove(0)
{
	movex = 0;
	movez = 0;
}

void AutoPlay::Wakeup()
{
	thinking.store(true);
	brain = std::thread([=]()
	{
		while (thinking.load() == true)
		{
			OnThink();
			std::this_thread::sleep_for(std::chrono::microseconds(100000));
		}

	});
}

void AutoPlay::Sleep()
{
	thinking.store(false);
	if (brain.joinable() == true)
	{
		brain.detach();
	}
}

void AutoPlay::OnThink()
{
	if (EntityManager::instance().zoneLoaded.load() == false)
	{
		return;
	}
	auto playerman = &EntityManager::instance().players;
	playerman->Lock();
	auto mine = playerman->getMyPlayer();
	if (mine != nullptr)
	{
		MoveRandom(mine);
	}

	playerman->UnLock();
}

void AutoPlay::MoveRandom(Player *player)
{
	float minx = -0.1f;
	float minz = -0.1f;
	float maxx = 0.1f;
	float maxz = 0.1f;
	Vector3 pos = player->GetPosition();

	if (directmove >= 0)
	{
		directmove--;
		pos.x += movex;
		pos.z += movez;
	}

	if (pos.x < 0.1f)
	{
		directmove = 0;
		minx = - pos.x;
	} else if (pos.x > 299.5f)
	{
		directmove = 0;
		maxx = 300.0f - pos.x;
	}

	if (pos.z < 0.1f)
	{
		directmove = 0;
		minz = - pos.z;
	} else if (pos.z > 299.5f)
	{
		directmove = 0;
		maxz = 300.0f - pos.z;
	}

	if (directmove <= 0)
	{
		movex  = getRandomFloatInRange(minx, maxx);
		movez  = getRandomFloatInRange(minz, maxz);
		directmove = (int)getRandomFloatInRange(0, 10) + 4;
	}

	int heading = player->calc_heading(pos.x, pos.z);
	player->UpdatePosition(heading, pos);
	SendUpdatePosition(player);
}

void AutoPlay::SendUpdatePosition(Player* player)
{
	NetworkMessage::CS_UpdatePosition_FBS req;
	req.sessionid = player->GetSessionID();
	req.head = player->GetHeading();
	req.position = EntityManager::instance().client.Vector3Convert(player->GetPosition());
	EntityManager::instance().client.Send(NetworkMessage::ClientPackets::ClientPackets_CS_UpdatePosition, &req);
}
