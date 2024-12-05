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
#include "TaskQueueThread.h"


class WebSocketServer
{
public:
	WebSocketServer();
	void Start(uint16_t port,int threadsize);
	void Stop();
	void Send(const websocketpp::connection_hdl hdl, const std::shared_ptr<std::vector<uint8_t>> shared_data);
	void SendBroadcast(const std::vector<websocketpp::connection_hdl>& clients, const std::shared_ptr<std::vector<uint8_t>> shared_data);

	ClientManager* GetClientManager();
	websocketpp::server<websocketpp::config::asio>* GetSocketpp();

private: 
	void OnConnect(const websocketpp::connection_hdl hdl);
	void OnDisconnect(const websocketpp::connection_hdl hdl);
	void OnRecive(const websocketpp::connection_hdl hdl, const websocketpp::server<websocketpp::config::asio>::message_ptr recvdata);

private:
	std::vector<std::thread> socketioThread;
	TaskQueueThread taskQueueThread;
	TaskQueueThread sendQueueThread;
	ClientManager connectSessions;
	websocketpp::server<websocketpp::config::asio> socketInstance;
	
};

