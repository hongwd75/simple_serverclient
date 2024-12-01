#pragma once
#include"Player.h"

class AutoPlay
{
public:
	AutoPlay();
	void Wakeup();
	void Sleep();
	void OnThink();
	void MoveRandom(Player* player);

private:
	void SendUpdatePosition(Player* player);

private:
	int directmove;
	float movex, movez;
	std::thread brain;
	std::atomic<bool> thinking;
};

