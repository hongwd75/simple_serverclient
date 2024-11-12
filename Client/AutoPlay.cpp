#include "AutoPlay.h"
#include "EntityManager.h"
#include <iostream>
#include <cmath>
#include <random>



float getRandomFloatInRange(float lower, float upper) {
	static std::random_device rd;
	static std::mt19937 generator(rd());  // Mersenne Twister 엔진 사용
	std::uniform_real_distribution<float> distribution(lower, upper);
	return distribution(generator);
}


AutoPlay::AutoPlay() : thinking(false)
{
}

void AutoPlay::Wakeup()
{
	thinking.store(true);
	brain = std::thread([=]()
	{
		while (thinking.load() == true)
		{
			OnThink();
			std::this_thread::sleep_for(std::chrono::microseconds(500));
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
	float minx = -0.5f;
	float minz = -0.5f;
	float maxx = 0.5f;
	float maxz = 0.5f;
	Vector3 pos = player->GetPosition();

	if (pos.x < 0.5f)
	{
		minx = - pos.x;
	} else if (pos.x > 299.5f)
	{
		maxx = 300.0f - pos.x;
	}

	if (pos.z < 0.5f)
	{
		minz = - pos.z;
	} else if (pos.z > 299.5f)
	{
		maxz = 300.0f - pos.z;
	}

	pos.x += getRandomFloatInRange(minx, maxx);
	pos.z += getRandomFloatInRange(minz, maxz);

	int heading = player->calc_heading(pos.x, pos.z);
	player->UpdatePosition(heading, pos);
}

void AutoPlay::SendUpdatePosition(Player* player)
{
	NetworkMessage::CS_UpdatePosition_FBS req;
	req.sessionid = player->GetSessionID();
	req.head = player->GetHeading();
	req.position = EntityManager::instance().client.Vector3Convert(player->GetPosition());
	EntityManager::instance().client.Send(NetworkMessage::ClientPackets::ClientPackets_CS_UpdatePosition, &req);
}
