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

	void MoveRandom();	// �׽�Ʈ �ڵ�

	const int GetSessionID() {	return session;	}
	const Vector3 GetPosition()	{	return position; }
	const int GetHeading()	{	return heading;	}
	const std::string GetName() { return name; }

public:
	float calc_heading(float x, float z); // �ٶ󺸴� ���� ó��

private:
	int session;
	int realm;				// 1,2,3    0:�� �߸�
	std::string name;
	Vector3 position;
	int heading = 0;

	// �ӽð�
	int hp = 1000;
	int maxhp = 1000;
};

