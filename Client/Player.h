#pragma once
#include<mutex>
#include<string>
#include"structs.h"


class Player
{
public:
	Player(int _session, std::string _name,int _realm);
	void SetRealm(int _realm);
	void UpdatePosition(int _heading,Vector3 pos);

	void MoveRandom();	// 테스트 코드

	const int GetSessionID() {	return session;	}
	const Vector3 GetPosition()	{	return position; }
	const int GetHeading()	{	return heading;	}
	const std::string GetName() { return name; }

public:
	float calc_heading(float x, float z); // 바라보는 방향 처리

private:
	int session;
	int realm;				// 1,2,3    0:은 중립
	std::string name;
	Vector3 position;
	int heading = 0;

	// 임시값
	int hp = 1000;
	int maxhp = 1000;
};

