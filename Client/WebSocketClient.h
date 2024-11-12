#pragma once
#include "..\common\packets\networkmessages_generated.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "structs.h"

typedef websocketpp::client<websocketpp::config::asio> client;


class WebSocketClient
{
public:
    WebSocketClient();
    void connect(const std::string& uri);
    void close();

    void send_message(const std::string& message);
    template<typename T>
    void Send(NetworkMessage::ClientPackets packetid, const T* message);
    std::shared_ptr<NetworkMessage::Vector3> Vector3Convert(Vector3 pos);

public:
    void SendLoginReq(const char* account, const char* pwd); // 접속하면 바로 보낸다.

private:
    void on_open(websocketpp::connection_hdl hdl);
    void OnRecive(websocketpp::connection_hdl, client::message_ptr msg);
    void OnBinaryRecive(const char* buffer);


private:
    void HANDLE_PACKET_SC_LoginAns(const NetworkMessage::SC_LoginAns* packet);
    void HANDLE_PACKET_SC_AccountInfo(const NetworkMessage::SC_AccountInfo* packet);
    void HANDLE_PACKET_SC_CreatePlayer(const NetworkMessage::SC_CreatePlayer* packet);
    void HANDLE_PACKET_SC_CreatePlayers(const NetworkMessage::SC_CreatePlayers* packet);
    void HANDLE_PACKET_SC_UpdatePosition(const NetworkMessage::SC_UpdatePosition* packet);
    void HANDLE_PACKET_SC_WorldEnterNotify(const NetworkMessage::SC_WorldEnterNotify* packet);

private:
    client client_instance;
    websocketpp::connection_hdl connection_hdl;

    std::mutex cout_mutex;
    std::thread client_thread;
};


//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
template<typename T>
inline void WebSocketClient::Send(NetworkMessage::ClientPackets packetid, const T* message)
{
    flatbuffers::FlatBufferBuilder builder;
    auto serialized_message = T::TableType::Pack(builder,message);
    builder.Finish(serialized_message);

    std::vector<uint8_t> packet_data(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize());

    NetworkMessage::PacketWrapper_FBS pack;
    pack.type = packetid;
    pack.packet = packet_data;

    flatbuffers::FlatBufferBuilder builder2;
    auto serialized_message2 = NetworkMessage::PacketWrapper::Pack(builder2,&pack);
    builder2.Finish(serialized_message2);

    try
    {
        if (auto conn = connection_hdl.lock())
        {
            client_instance.send(conn, builder2.GetBufferPointer(), builder2.GetSize(), websocketpp::frame::opcode::binary);
        }
    }
    catch (const std::exception&)
    {

    }
}