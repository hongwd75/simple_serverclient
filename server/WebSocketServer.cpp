#include "WebSocketServer.h"
#include "ServerMain.h"
#include "EventCallbackManager.h"


WebSocketServer::WebSocketServer() : connectSessions(this)
{
	socketInstance.init_asio();
	socketInstance.set_open_handler(std::bind(&WebSocketServer::OnConnect, this, std::placeholders::_1));
	socketInstance.set_close_handler(std::bind(&WebSocketServer::OnDisconnect, this, std::placeholders::_1));
	socketInstance.set_fail_handler(std::bind(&WebSocketServer::OnDisconnect, this, std::placeholders::_1));
	socketInstance.set_message_handler(std::bind(&WebSocketServer::OnRecive, this, std::placeholders::_1, std::placeholders::_2));
}

void WebSocketServer::Start(uint16_t port, int threadsize)
{
	socketInstance.clear_access_channels(websocketpp::log::alevel::all); 
	socketInstance.set_access_channels(websocketpp::log::alevel::frame_payload);
	socketInstance.listen(port);
	socketInstance.start_accept();
	
	taskQueueThread.start();
	sendQueueThread.start();

	for (int i = 0; i < threadsize; ++i) 
	{
		socketioThread.emplace_back([this] { socketInstance.run(); });
	}
}

void WebSocketServer::Stop()
{
	taskQueueThread.stop();
	sendQueueThread.stop();
	socketInstance.stop();
	for (auto& t : socketioThread) {
		if (t.joinable() == true)
		{
			t.join();
		}
	}
	socketioThread.clear();
}



void WebSocketServer::Send(const websocketpp::connection_hdl hdl, const std::shared_ptr<std::vector<uint8_t>> shared_data)
{
	try
	{
		socketInstance.send(hdl, shared_data->data(), shared_data->size(), websocketpp::frame::opcode::binary);
	}
	catch (const std::exception& e) {
		// 예외 처리
		std::cerr << "Send exception: " << e.what() << std::endl;
		//EventCallbackManager::instance()->Notify(EventCallback::eTrigger::RemoveClientNetworkHandle, nullptr, new ForceDisconnedEventArgs(hdl));
	}
}

void WebSocketServer::SendBroadcast(const std::vector<websocketpp::connection_hdl>& clients, const std::shared_ptr<std::vector<uint8_t>> shared_data)
{
	if (clients.empty() == true)
	{
		return;
	}

//	auto message = socketInstance.get_con_from_hdl(clients[0])->get_message(websocketpp::frame::opcode::binary, shared_data->size());
//	message->append_payload(shared_data->data(), shared_data->size());

	for (auto& client : clients) 
	{
		try
		{
			//socketInstance.send(client, message);
			socketInstance.send(client, shared_data->data(), shared_data->size(), websocketpp::frame::opcode::binary);
		}
		catch (const std::exception& e) {
			std::cerr << "Broadcast exception: " << e.what() << std::endl;
			//EventCallbackManager::instance()->Notify(EventCallback::eTrigger::RemoveClientNetworkHandle, nullptr, new ForceDisconnedEventArgs(client));
		}
	}
	return;

	{
		for (auto& client : clients) {
			sendQueueThread.addTask(0,
				{"SendBroadcast", 
				[this, client, shared_data = std::move(shared_data)]
				{
					try
					{
						//socketInstance.send(client, message);
						socketInstance.send(client, shared_data->data(), shared_data->size(), websocketpp::frame::opcode::binary);
					}
					catch (const std::exception& e) {
						std::cerr << "Broadcast exception: " << e.what() << std::endl;
						//EventCallbackManager::instance()->Notify(EventCallback::eTrigger::RemoveClientNetworkHandle, nullptr, new ForceDisconnedEventArgs(client));
					}
				} 
				});
		}
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

void WebSocketServer::OnConnect(const websocketpp::connection_hdl hdl)
{
	connectSessions.AddSession(hdl);
}

void WebSocketServer::OnDisconnect(const websocketpp::connection_hdl hdl)
{
	connectSessions.RemoveSession(hdl);
}

void WebSocketServer::OnRecive(const websocketpp::connection_hdl hdl, const websocketpp::server<websocketpp::config::asio>::message_ptr recvdata)
{
	if (recvdata->get_opcode() == websocketpp::frame::opcode::binary)
	{
		auto payload_ptr = std::vector<uint8_t>(recvdata->get_payload().begin(), recvdata->get_payload().end());

		taskQueueThread.addTask(0,
			{"",
			[this, hdl, payload_ptr = std::move(payload_ptr)]
			{
				auto packetWarpper = NetworkMessage::GetPacketWrapper(payload_ptr.data());
				if (packetWarpper != nullptr)
				{
					auto packetType = packetWarpper->type();
					auto packetData = packetWarpper->packet();
					connectSessions.ExecuteWithLock([&, packetType = packetType, packetData = std::move(packetData)]()
						{
							if (auto target = connectSessions.GetClient(hdl))
							{
								target->OnRecive(packetType, packetData);
							}
						});
				}
			} 
			});
	}
	else
	{
		std::string text_payload = recvdata->get_payload();
		std::cout << "수신한 텍스트 메시지: " << text_payload << std::endl;
	}
}


