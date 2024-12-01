#pragma once
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <string>
#include "PacketHandler.h"

class LockCount {
public:
    std::mutex mtx;
    int count = 0;
};

class LockManager;

class LoginReqHandler : public  PacketHandler
{
public:
    void HandlePacket(Client* user, const ::flatbuffers::Vector<uint8_t>* recvdata) override;

private:
    static LockManager lockman;

    void sendLoginAns(Client* user, int errorcode);
    void check_login(Client* user, const std::string& id, const std::string& pwd);
};

//===========================================================
class LockManager {
public:
    void EnterLock(const std::string& accountName) {
        std::string accName = accountName;
        if (accName.empty()) {
            std::cerr << "(Enter) No account name" << std::endl;
            accName = "";  // 빈 문자열로 처리
        }

        std::shared_ptr<LockCount> lockObj = nullptr;

        { // 스코프를 이용해 전체 mutex 범위를 제한
            std::unique_lock<std::mutex> lock(mapMutex);

            // map에서 lock 객체 가져오기 또는 생성하기
            auto it = m_locks.find(accName);
            if (it == m_locks.end()) {
                lockObj = std::make_shared<LockCount>();
                m_locks[accName] = lockObj;
            }
            else {
                lockObj = it->second;
            }

            if (!lockObj) {
                std::cerr << "(Enter) No lock object for account: '" << accName << "'" << std::endl;
            }
            else {
                // 잠금 횟수 증가
                lockObj->count++;
            }
        }

        // lockObj의 mutex 잠금
        if (lockObj) {
            lockObj->mtx.lock();
        }
    }

    void ExitLock(const std::string& accountName) {
        std::shared_ptr<LockCount> lockObj = nullptr;

        { // 스코프 제한을 위한 중괄호
            std::unique_lock<std::mutex> lock(mapMutex);
            auto it = m_locks.find(accountName);

            if (it != m_locks.end()) {
                lockObj = it->second;

                // lockObj가 더 이상 필요하지 않으면 제거
                if (--lockObj->count == 0) {
                    m_locks.erase(it);
                }
            }
        }

        // lockObj의 mutex 잠금 해제
        if (lockObj) {
            lockObj->mtx.unlock();
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<LockCount>> m_locks;
    std::mutex mapMutex;
};