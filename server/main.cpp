// server.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <iostream>
#include "ConsoleDisplay.h"
#include "ServerMain.h"
#include "EventCallbackManager.h"


class testEventArgs : public EventArgs {
public:
	std::string message;
	testEventArgs(const std::string& name) : message(name) {}
};

class TestNotify
{
public:
	TestNotify()
	{
		EventCallbackManager::instance()->BindEvent(this, EventCallback::eTrigger::WriteLog, &TestNotify::OnWrite);
	}
	~TestNotify()
	{
		EventCallbackManager::instance()->UnbindEvent(this, EventCallback::eTrigger::WriteLog, &TestNotify::OnWrite);
	}
	void OnWrite(void* sender, EventArgs* args)
	{
		testEventArgs* pMsg = (testEventArgs*)args;
		if (pMsg != nullptr)
		{
			ConsoleDisplay::Write(ConsoleDisplay::TYPE::Normal, pMsg->message.c_str());
		}
	}
};

int main()
{
	TestNotify test;
	EventCallbackManager::instance()->Notify(EventCallback::eTrigger::WriteLog, nullptr, new testEventArgs("하이루"));

	ServerMain server;
    
	std::string command;
    while (true)
    {
		std::cout << "Command: ";
		std::getline(std::cin, command);
		if (command == "exit")
		{
			server.Stop();
			break;
		}
    }
}
