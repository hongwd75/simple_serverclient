#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include "..\common\packets\networkmessages_generated.h"
#include "StatesEnums.h"
#include "Account.h"
#include <websocketpp/server.hpp>
#include "ConsoleDisplay.h"


class WebSocketServer;

class Client
{
public:
	Client(websocketpp::connection_hdl hdl,int sessionid, WebSocketServer* svr);
	void RestSocket();
	void SetSocket(websocketpp::connection_hdl sock);
	void SetAccount(Account* info);
	void SetPlayerState(Enums::ClientState st);

	websocketpp::connection_hdl GetSocket();
	int GetSessionID() { return SessionID; }

	Enums::ClientState GetPlayerState();
	Account* GetAccount();

public:
	template<typename T>
	void Send(NetworkMessage::ServerPackets packetid, const T* message);
	void OnRecive(const uint16_t packetType, const ::flatbuffers::Vector<uint8_t>* packetData);

private:
	void SendInternal(const char* data, size_t size);
	void Log(const char* msg);

private:
	int SessionID = 0;
	Account m_account;

	Enums::ClientState clientState = Enums::ClientState::NotConnected;
	Enums::ConnectState networkState = Enums::ConnectState::Green;
	websocketpp::connection_hdl connection;
	WebSocketServer* _svr;
};


//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
template<typename T>
inline void Client::Send(NetworkMessage::ServerPackets packetid, const T* message)
{
	flatbuffers::FlatBufferBuilder builder;
	auto serialized_message = T::TableType::Pack(builder, message);
	builder.Finish(serialized_message);

	std::vector<uint8_t> packet_data(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize());

	NetworkMessage::PacketWrapper_FBS pack;
	pack.type = packetid;
	pack.packet = packet_data;


	flatbuffers::FlatBufferBuilder builder2;
	auto serialized_message2 = NetworkMessage::PacketWrapper::Pack(builder2, &pack);
	builder2.Finish(serialized_message2);

	SendInternal(reinterpret_cast<const char*>(builder2.GetBufferPointer()), builder2.GetSize());
}
#endif // CLIENT_H