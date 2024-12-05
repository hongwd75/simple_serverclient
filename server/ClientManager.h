#pragma once
#include "Client.h"
#include <websocketpp/server.hpp>
#include<unordered_map>
#include<list>
#include<mutex>
#include "EventCallbackManager.h"

class WebSocketServer;
struct TaggedTask {
	std::string tag;
	std::function<void()> task;
};

class ForceDisconnedEventArgs : public EventArgs 
{
public:
	websocketpp::connection_hdl disconnectObj;
	ForceDisconnedEventArgs(websocketpp::connection_hdl obj) : disconnectObj(obj) {}
};

struct connection_hdl_hash {
	std::size_t operator()(const websocketpp::connection_hdl& hdl) const {
		return std::hash<void*>()(hdl.lock().get());
	}
};

struct connection_hdl_equal {
	bool operator()(const websocketpp::connection_hdl& lhs,
		const websocketpp::connection_hdl& rhs) const {
		return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
	}
};

class ClientManager
{
	const int MaxUserSize = 10000;
public:
	ClientManager(WebSocketServer* server);
	~ClientManager();
	void OnForceDisconnectEvent(void* sender, EventArgs* args);

public:
	bool AddSession(websocketpp::connection_hdl hdl);
	bool AddLoginedUser(Client* client);
	bool ChangeSocketSession(uint64_t uid, websocketpp::connection_hdl newsocket);

	void RemoveSession(websocketpp::connection_hdl hdl);
	void RemoveSessionByUUID(uint64_t uid);

public:
	template<typename Func>
	void ExecuteWithLock(Func&& func) {
		std::lock_guard<std::mutex> lock(session_mutex);
		func();
	}

	std::vector<Client*> GetRangeUsers(Structs::Vector3 pos, int range);
	std::vector< websocketpp::connection_hdl> GetConnectVector(const std::vector<Client*> list, Client* remove);
public:
	Client *GetClientByUID(uint64_t uid);
	Client *GetClient(websocketpp::connection_hdl hdl);
	Client* GetClientBySession(int sessionid);

public:
	int GetClientSize();

private:
	std::unordered_map<uint64_t,int> Clients;
	std::unordered_map<websocketpp::connection_hdl,	int, connection_hdl_hash, connection_hdl_equal> sock2Client;

	std::queue<int> emptyClientSlot;
	std::vector<Client*> ClientSlot;
	std::mutex session_mutex;
	WebSocketServer* server;
};

