// Client.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "EntityManager.h"


int main(int argc, char* argv[])
{

    std::string accountid = "normal_1";
    std::string password = "1";

    if (argc > 2)
    {
        accountid = argv[1];
        password = argv[2];
    }

    EntityManager::instance().accountid = accountid;
    EntityManager::instance().password = password;

    std::cout << "Hello World!\n";
    EntityManager::instance().ConnectToServer();
    std::string command;
    
    while(true) 
    {
        std::cout << "Enter 'login [username]' to log in or 'exit' to quit: ";
        std::getline(std::cin, command);

        if (command == "exit") {
            EntityManager::instance().CloseSocket();
            break;
        }
    }
    return 0;
}
