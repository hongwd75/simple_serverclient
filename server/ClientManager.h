#pragma once
#include "Client.h"
#include <websocketpp/server.hpp>
#include<unordered_map>
#include<list>
#include<mutex>

class WebSocketServer;

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

public:
	bool AddSession(websocketpp::connection_hdl hdl);
	bool AddLoginedUser(Client* client);
	bool ChangeSocketSession(uint64_t uid, websocketpp::connection_hdl newsocket);

	void RemoveSession(websocketpp::connection_hdl hdl);
	void RemoveSeesionByUUID(uint64_t uid);

public:
	void Lock() { session_mutex.lock(); }
	void Unlock() { session_mutex.unlock(); }
	std::vector<Client*> GetRangeUsers(Structs::Vector3 pos, int range);

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
	std::vector<std::unique_ptr<Client>> ClientSlot;
	std::mutex session_mutex;
	WebSocketServer* server;
};

