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

    // atan2를 사용하여 각도를 구합니다. 결과는 라디안 단위입니다.
    float angleInRadians = std::atan2(deltaZ, deltaX);

    // 원하는 경우, 각도를 degree로 변환할 수 있습니다.
    float angleInDegrees = angleInRadians * (180.0f / M_PI);

    // 각도는 기본적으로 -180 ~ 180 사이의 값으로 나옵니다.
    if (angleInDegrees < 0) {
        angleInDegrees += 360.0f;  // 0 ~ 360 범위로 변환
    }

    return angleInDegrees;  // degree 단위의 각도 반환
}
