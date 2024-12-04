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
	
	for (int i = 0; i < threadsize; ++i) 
	{
		workerThreads.emplace_back(&WebSocketServer::WorkerThread, this);
		socketioThread.emplace_back([this] { socketInstance.run(); });
	}
}

void WebSocketServer::Stop()
{
	running = false;
	queueCv.notify_all();

	// 작업 쓰레드 종료
	for (auto& thread : workerThreads) {
		if (thread.joinable()) {
			thread.join();
		}
	}

	socketInstance.stop();
	for (auto& t : socketioThread) {
		if (t.joinable() == true)
		{
			t.join();
		}
	}
	socketioThread.clear();
}

void WebSocketServer::WorkerThread() {
	while (running) {
		TaggedTask taggedTask;
		int queuesize = 0;
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			queueCv.wait(lock, [this] { return !taskQueue.empty() || !running; });

			if (!running && taskQueue.empty()) {
				break;
			}

			taggedTask = std::move(taskQueue.front());
			taskQueue.pop();
			queuesize = taskQueue.size();
		}

		if (taggedTask.task) {
			auto start = std::chrono::high_resolution_clock::now();
			taggedTask.task();
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

			std::ostringstream oss;
			oss << "Task 실행 시간: " << duration.count() << "ms | 큐 : "<< queuesize  <<"\n";
			OutputDebugStringA(oss.str().c_str());
		}
	}
}


void WebSocketServer::Send(websocketpp::connection_hdl hdl, const std::vector<uint8_t>& data)
{
	auto shared_data = std::make_shared<std::vector<uint8_t>>(data);
	try
	{
		socketInstance.send(hdl, shared_data->data(), shared_data->size(), websocketpp::frame::opcode::binary);
	}
	catch (const std::exception& e) {
		// 예외 처리
		std::cerr << "Send exception: " << e.what() << std::endl;
		//EventCallbackManager::instance()->Notify(EventCallback::eTrigger::RemoveClientNetworkHandle, nullptr, new ForceDisconnedEventArgs(hdl));
	}
	return;
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		taskQueue.push(
			{ "Send",[this, hdl, shared_data] {
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
			});
	}
	queueCv.notify_one();
}

void WebSocketServer::SendBroadcast(const std::vector<websocketpp::connection_hdl>& clients, const std::vector<uint8_t>& data)
{
	auto shared_data = std::make_shared<std::vector<uint8_t>>(data);
	for (auto& client : clients) 
	{

		try
		{
			if (socketInstance.get_con_from_hdl(client)->get_buffered_amount() > 100)
			{
				std::cout << "datasize:" << shared_data->size() << "buffered_amount " << socketInstance.get_con_from_hdl(client)->get_buffered_amount() << std::endl;
			}

			socketInstance.send(client, shared_data->data(), shared_data->size(), websocketpp::frame::opcode::binary);

		}
		catch (const std::exception& e) {
			// 예외 처리
			std::cerr << "Broadcast exception: " << e.what() << std::endl;
			//EventCallbackManager::instance()->Notify(EventCallback::eTrigger::RemoveClientNetworkHandle, nullptr, new ForceDisconnedEventArgs(client));
		}
	}
	return;
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		
		for (auto& client : clients) {
			taskQueue.push(
				{"SendBroadcast", 
				[this, client, shared_data] 
				{
					try
					{
						if (socketInstance.get_con_from_hdl(client)->get_buffered_amount() > 100)
						{
							std::cout << "datasize:" << shared_data->size() << "buffered_amount " << socketInstance.get_con_from_hdl(client)->get_buffered_amount() << std::endl;
						}

						socketInstance.send(client, shared_data->data(), shared_data->size(), websocketpp::frame::opcode::binary);

					}
					catch (const std::exception& e) {
						// 예외 처리
						std::cerr << "Broadcast exception: " << e.what() << std::endl;
						//EventCallbackManager::instance()->Notify(EventCallback::eTrigger::RemoveClientNetworkHandle, nullptr, new ForceDisconnedEventArgs(client));
					}
				} 
				});
		}
	}
	queueCv.notify_all();
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
		auto payload_ptr = std::make_shared<std::vector<uint8_t>>(recvdata->get_payload().begin(), recvdata->get_payload().end());
		
		//if (packetWarpper != nullptr)
		{
			{
				std::lock_guard<std::mutex> lock(queueMutex);
				taskQueue.push(
					{"",
					[this, hdl, payload_ptr = std::move(payload_ptr)]
					{
						auto packetWarpper = NetworkMessage::GetPacketWrapper(payload_ptr->data());
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
			queueCv.notify_one();
		}
	}
	else
	{
		std::string text_payload = recvdata->get_payload();
		std::cout << "수신한 텍스트 메시지: " << text_payload << std::endl;
	}
}


