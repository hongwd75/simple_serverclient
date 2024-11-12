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

class WebSocketServer
{
public:
	WebSocketServer();
	void Start(uint16_t port);
	void Stop();
	void Send(websocketpp::connection_hdl hdl, const char* data, size_t datasize);

	ClientManager* GetClientManager();
	websocketpp::server<websocketpp::config::asio>* GetSocketpp();

private: 
	void OnConnect(websocketpp::connection_hdl hdl);
	void OnDisconnect(websocketpp::connection_hdl hdl);
	void OnRecive(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr recvdata);

private:
	std::thread socketioThread;
	websocketpp::server<websocketpp::config::asio> socketInstance;
	ClientManager connectSessions;
};

