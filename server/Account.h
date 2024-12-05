#pragma once
#include<string>
#include<mutex>
#include"StatesEnums.h"
class Account
{
public:
	uint64_t UID = 0;
    std::string AccountName = "";


    int realm = 0;
    int heading = 0;
    Structs::Vector3 position;

public:
    Account& operator=(const Account& other) {
        if (this == &other) {
            return *this;
        }

        UID = other.UID;
        AccountName = other.AccountName;
        heading = other.heading;
        realm = other.realm;
        position = other.position;
        return *this;
    }

public:
    void UpdatePosition(int head, float x, float y, float z)
    {
        std::lock_guard< std::mutex> lock(_mutex);
        heading = head;
        position.x = x;
        position.y = y;
        position.z = z;
    }
private:
    std::mutex _mutex;
};

