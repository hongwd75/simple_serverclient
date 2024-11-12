#include "ServerMain.h"
#include "ConsoleDisplay.h"

ServerMain* ServerMain::_intance = nullptr;
ServerMain* ServerMain::instance()
{
#ifdef _DEBUG
	if (ServerMain::_intance == nullptr)
	{
		ConsoleDisplay::Write(ConsoleDisplay::TYPE::Error, "    ServerMain 초기화 없이 요청하고 있음");
		DebugBreak();
	}
#endif
	return ServerMain::_intance;
}

TaskManager* ServerMain::TaskMan()
{
	return task_manager;
}

WebSocketServer* ServerMain::SocketMan()
{
	return &serversocket;
}


ServerMain::ServerMain()
{
	_intance = this;
	size_t core_count = boost::thread::hardware_concurrency();

	ConsoleDisplay::WriteFormat(ConsoleDisplay::TYPE::Normal, "스레드 풀 생성(%d core)....", core_count);
	task_manager = new TaskManager(core_count);

	ConsoleDisplay::Write(ConsoleDisplay::TYPE::Normal, "서버 소켓 초기화 진행");

	try
	{
		serversocket.Start(1000);
	}
	catch (const std::exception& e)
	{
		ConsoleDisplay::Write(ConsoleDisplay::TYPE::Error, e.what());
	}

}

ServerMain::~ServerMain()
{
	_intance = nullptr;
}

void ServerMain::Stop()
{
	ConsoleDisplay::Write(ConsoleDisplay::TYPE::Normal, "서버 중지..");
	serversocket.Stop();
}
