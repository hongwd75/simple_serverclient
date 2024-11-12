#pragma once
#include <mutex>
#include "ThreadPool.h"
#include "WebSocketServer.h"
#include "PacketHandlerFactory.h"
class ServerMain
{
	/* 서버 설정 */
public:
	const int server_port = 1000;
	static ServerMain* instance();
	TaskManager* TaskMan();
	WebSocketServer* SocketMan();
public:
	ServerMain();
	~ServerMain();

	void Stop();
	

private:
	static ServerMain* _intance;
	TaskManager *task_manager;
	WebSocketServer serversocket;
	PacketHandlerFactory packethandlerfactory;
};

