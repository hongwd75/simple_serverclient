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
	// 작업 쓰레드 생성
	for (int i = 0; i < threadsize; ++i) {
		workerThreads.emplace_back(&WebSocketServer::WorkerThread, this);
	}

	socketInstance.clear_access_channels(websocketpp::log::alevel::all); 
	socketInstance.set_error_channels(websocketpp::log::elevel::all);  // 모든 에러 로그
	socketInstance.listen(port);
	socketInstance.start_accept();

	socketioThread.emplace_back([this] { socketInstance.run(); });
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
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			queueCv.wait(lock, [this] { return !taskQueue.empty() || !running; });

			if (!running && taskQueue.empty()) {
				break;
			}

			task = std::move(taskQueue.front());
			taskQueue.pop();
		}

		if (task) {
			task();
		}
	}
}


void WebSocketServer::Send(websocketpp::connection_hdl hdl, const std::vector<uint8_t>& data)
{
	auto shared_data = std::make_shared<std::vector<uint8_t>>(data);
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		taskQueue.push([this, hdl, shared_data] {
			try {
				socketInstance.send(hdl, shared_data->data(), shared_data->size(), websocketpp::frame::opcode::binary);
			}
			catch (const websocketpp::exception& e) {
				// 예외 처리
				std::cerr << "Send exception: " << e.what() << std::endl;
			}
			});
	}
	queueCv.notify_one();
}

void WebSocketServer::SendBroadcast(std::vector<websocketpp::connection_hdl>& clients, const std::vector<uint8_t>& data)
{
	auto shared_data = std::make_shared<std::vector<uint8_t>>(data);
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		for (auto& client : clients) {
			taskQueue.push([this, client, shared_data] {
				try {
					socketInstance.send(client, shared_data->data(), shared_data->size(), websocketpp::frame::opcode::binary);
				}
				catch (const websocketpp::exception& e) {
					// 예외 처리
					std::cerr << "Broadcast exception: " << e.what() << std::endl;
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
		auto packetWarpper = NetworkMessage::GetPacketWrapper(recvdata->get_payload().data());
		if (packetWarpper != nullptr)
		{
			auto packetType = packetWarpper->type();
			auto packetData = packetWarpper->packet();
			
			connectSessions.ExecuteWithLock([&]() 
				{
					auto target = connectSessions.GetClient(hdl);
					if (target != nullptr)
					{
						target->OnRecive(packetType, packetData);
					}
				});

		}
	}
	else
	{
		std::string text_payload = recvdata->get_payload();
		std::cout << "수신한 텍스트 메시지: " << text_payload << std::endl;
	}
}


