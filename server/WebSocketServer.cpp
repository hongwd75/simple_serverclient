#include "WebSocketServer.h"
#include "ServerMain.h"

WebSocketServer::WebSocketServer() : connectSessions(this)
{
	socketInstance.set_open_handler(std::bind(&WebSocketServer::OnConnect, this, std::placeholders::_1));
	socketInstance.set_close_handler(std::bind(&WebSocketServer::OnDisconnect, this, std::placeholders::_1));
	socketInstance.set_fail_handler(std::bind(&WebSocketServer::OnDisconnect, this, std::placeholders::_1));
	socketInstance.set_message_handler(std::bind(&WebSocketServer::OnRecive, this, std::placeholders::_1, std::placeholders::_2));
	socketInstance.init_asio();
}

void WebSocketServer::Start(uint16_t port)
{
	socketInstance.clear_access_channels(websocketpp::log::alevel::all);
	socketInstance.set_error_channels(websocketpp::log::elevel::all);  // 모든 에러 로그
	socketInstance.listen(port);
	socketInstance.start_accept();
	socketioThread = std::thread([&]() {
		socketInstance.run();
		});
}

void WebSocketServer::Stop()
{
	socketInstance.stop();
	if (socketioThread.joinable() == true)
	{
		socketioThread.join();
	}
}

void WebSocketServer::Send(websocketpp::connection_hdl hdl, const char* data, size_t datasize)
{
	try
	{
		socketInstance.send(hdl, data, datasize, websocketpp::frame::opcode::binary);
	}
	catch (const std::exception&)
	{
		connectSessions.RemoveSession(hdl);
	}
}

ClientManager* WebSocketServer::GetClientManager()
{
	return &connectSessions;
}

websocketpp::server<websocketpp::config::asio>* WebSocketServer::GetSocketpp()
{
	return &socketInstance;
}

void WebSocketServer::OnConnect(websocketpp::connection_hdl hdl)
{
	connectSessions.AddSession(hdl);

	std::stringstream message;
	message << "[C]Connects: " << connectSessions.GetClientSize() << "\n";

	std::cout << message.str();

	OutputDebugStringA(message.str().c_str());
}

void WebSocketServer::OnDisconnect(websocketpp::connection_hdl hdl)
{
	connectSessions.RemoveSession(hdl);
	std::stringstream message;
	message << "[D]Connects: " << connectSessions.GetClientSize() << "\n";

	std::cout << message.str();

	OutputDebugStringA(message.str().c_str());
}

void WebSocketServer::OnRecive(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr recvdata)
{
	if (recvdata->get_opcode() == websocketpp::frame::opcode::binary)
	{
		auto packetWarpper = NetworkMessage::GetPacketWrapper(recvdata->get_payload().data());
		if (packetWarpper != nullptr)
		{
			auto packetType = packetWarpper->type();
			auto packetData = packetWarpper->packet();
			
			connectSessions.Lock();
			auto target = connectSessions.GetClient(hdl);
			if (target != nullptr)
			{
				target->OnRecive(packetType, packetData);
			}
			connectSessions.Unlock();
		}
	}
	else
	{
		std::string text_payload = recvdata->get_payload();
		std::cout << "수신한 텍스트 메시지: " << text_payload << std::endl;
	}
}


