#include "Player.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


Player::Player(int _session, std::string _name, int _realm) 
	: session(_session),name(std::move(name)),realm(_realm)
{

}

void Player::SetRealm(int _realm)
{
	realm = _realm;
}

void Player::UpdatePosition(int _heading, Vector3 pos)
{
	position = pos;
	heading = _heading;
}



float Player::calc_heading(float x,float z)
{
    float deltaX = x - position.x;
    float deltaZ = z - position.z;

    // atan2�� ����Ͽ� ������ ���մϴ�. ����� ���� �����Դϴ�.
    float angleInRadians = std::atan2(deltaZ, deltaX);

    // ���ϴ� ���, ������ degree�� ��ȯ�� �� �ֽ��ϴ�.
    float angleInDegrees = angleInRadians * (180.0f / M_PI);

    // ������ �⺻������ -180 ~ 180 ������ ������ ���ɴϴ�.
    if (angleInDegrees < 0) {
        angleInDegrees += 360.0f;  // 0 ~ 360 ������ ��ȯ
    }

    return angleInDegrees;  // degree ������ ���� ��ȯ
}
