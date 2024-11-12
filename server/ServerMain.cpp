#include "ServerMain.h"
#include "ConsoleDisplay.h"

ServerMain* ServerMain::_intance = nullptr;
ServerMain* ServerMain::instance()
{
#ifdef _DEBUG
	if (ServerMain::_intance == nullptr)
	{
		ConsoleDisplay::Write(ConsoleDisplay::TYPE::Error, "    ServerMain �ʱ�ȭ ���� ��û�ϰ� ����");
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

	ConsoleDisplay::WriteFormat(ConsoleDisplay::TYPE::Normal, "������ Ǯ ����(%d core)....", core_count);
	task_manager = new TaskManager(core_count);

	ConsoleDisplay::Write(ConsoleDisplay::TYPE::Normal, "���� ���� �ʱ�ȭ ����");

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
	ConsoleDisplay::Write(ConsoleDisplay::TYPE::Normal, "���� ����..");
	serversocket.Stop();
}
