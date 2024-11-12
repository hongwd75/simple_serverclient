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
            accName = "";  // �� ���ڿ��� ó��
        }

        std::shared_ptr<LockCount> lockObj = nullptr;

        { // �������� �̿��� ��ü mutex ������ ����
            std::unique_lock<std::mutex> lock(mapMutex);

            // map���� lock ��ü �������� �Ǵ� �����ϱ�
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
                // ��� Ƚ�� ����
                lockObj->count++;
            }
        }

        // lockObj�� mutex ���
        if (lockObj) {
            lockObj->mtx.lock();
        }
    }

    void ExitLock(const std::string& accountName) {
        std::shared_ptr<LockCount> lockObj = nullptr;

        { // ������ ������ ���� �߰�ȣ
            std::unique_lock<std::mutex> lock(mapMutex);
            auto it = m_locks.find(accountName);

            if (it != m_locks.end()) {
                lockObj = it->second;

                // lockObj�� �� �̻� �ʿ����� ������ ����
                if (--lockObj->count == 0) {
                    m_locks.erase(it);
                }
            }
        }

        // lockObj�� mutex ��� ����
        if (lockObj) {
            lockObj->mtx.unlock();
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<LockCount>> m_locks;
    std::mutex mapMutex;
};