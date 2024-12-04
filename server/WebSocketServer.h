#pragma once
#ifdef USE_TLS
#include <websocketpp/config/asio_tls.hpp>
#else
#include <websocketpp/config/asio_no_tls.hpp>
#endif

#include <websocketpp/server.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "ClientManager.h"

struct TaggedTask {
	std::string tag;
	std::function<void()> task;
};

class WebSocketServer
{
public:
	WebSocketServer();
	void Start(uint16_t port,int threadsize);
	void Stop();
	void Send(websocketpp::connection_hdl hdl, const std::vector<uint8_t>& data);
	void SendBroadcast(const std::vector<websocketpp::connection_hdl>& clients, const std::vector<uint8_t>& data);

	ClientManager* GetClientManager();
	websocketpp::server<websocketpp::config::asio>* GetSocketpp();

private: 
	void OnConnect(websocketpp::connection_hdl hdl);
	void OnDisconnect(websocketpp::connection_hdl hdl);
	void OnRecive(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr recvdata);
	void WorkerThread(); // 비동기 작업 처리 쓰레드 함수

private:
	std::vector<std::thread> socketioThread;
	std::vector<std::thread> workerThreads; // 작업 쓰레드
	websocketpp::server<websocketpp::config::asio> socketInstance;
	ClientManager connectSessions;

	// 작업 큐 및 동기화 변수
	std::queue<TaggedTask> taskQueue;
	std::mutex queueMutex;
	std::condition_variable queueCv;
	bool running = true;
};

