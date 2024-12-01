#include "WebSocketClient.h"
#include "PlayerManager.h"
#include "EntityManager.h"

#define HANDLE_PACKET(packetName, handlerCode) \
    case NetworkMessage::ServerPackets::ServerPackets_##packetName: { \
        auto packet = flatbuffers::GetRoot<NetworkMessage::packetName>(packetData->data()); \
        handlerCode(packet); \
        break; \
    }

WebSocketClient::WebSocketClient() {
    client_instance.init_asio();
    client_instance.set_open_handler(bind(&WebSocketClient::on_open, this, std::placeholders::_1));
    client_instance.set_message_handler(bind(&WebSocketClient::OnRecive, this, std::placeholders::_1, std::placeholders::_2));
    client_instance.clear_access_channels(websocketpp::log::alevel::all);
}

void WebSocketClient::connect(const std::string& uri) 
{
    websocketpp::lib::error_code ec;
    auto conn = client_instance.get_connection(uri, ec);

    if (ec) {
        std::cout << "Could not create connection: " << ec.message() << std::endl;
        return;
    }

    client_instance.connect(conn);
    client_thread = std::thread(&client::run, &client_instance);
}

void WebSocketClient::close() 
{
    websocketpp::lib::error_code ec;
    client_instance.close(connection_hdl, websocketpp::close::status::normal, "", ec);
    if (client_thread.joinable()) client_thread.join();
}

void WebSocketClient::SendLoginReq(const char *account,const char *pwd)
{
    NetworkMessage::CS_LoginReq_FBS req;
    req.id = account;
    req.pwd = pwd;;
    Send(NetworkMessage::ClientPackets::ClientPackets_CS_LoginReq, &req);
}

void WebSocketClient::on_open(websocketpp::connection_hdl hdl) 
{
    connection_hdl = hdl;
    std::cout << "Connected to server." << std::endl;
    SendLoginReq(EntityManager::instance().accountid.c_str(), EntityManager::instance().password.c_str());
}

void WebSocketClient::OnRecive(websocketpp::connection_hdl, client::message_ptr msg) {
    std::lock_guard<std::mutex> lock(cout_mutex);

    if (msg->get_opcode() == websocketpp::frame::opcode::binary)
    {
        OnBinaryRecive(msg->get_payload().data());
    }
    else
    {
        std::cout << "Received from server: " << msg->get_payload() << std::endl;
    }
}

void WebSocketClient::OnBinaryRecive(const char* buffer)
{
    auto packetWarpper = NetworkMessage::GetPacketWrapper(buffer);
    auto packetType = packetWarpper->type();
    auto packetData = packetWarpper->packet();
    switch (packetType)
    {
        HANDLE_PACKET(SC_LoginAns, HANDLE_PACKET_SC_LoginAns);
        HANDLE_PACKET(SC_AccountInfo, HANDLE_PACKET_SC_AccountInfo);
        HANDLE_PACKET(SC_CreatePlayer, HANDLE_PACKET_SC_CreatePlayer);
        HANDLE_PACKET(SC_CreatePlayers, HANDLE_PACKET_SC_CreatePlayers);
        HANDLE_PACKET(SC_UpdatePosition, HANDLE_PACKET_SC_UpdatePosition);
        HANDLE_PACKET(SC_WorldEnterNotify, HANDLE_PACKET_SC_WorldEnterNotify);
    }
}

void WebSocketClient::send_message(const std::string& message) {
    websocketpp::lib::error_code ec;
    client_instance.send(connection_hdl, message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cout << "Error sending message: " << ec.message() << std::endl;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 패킷 처리
void WebSocketClient::HANDLE_PACKET_SC_LoginAns(const NetworkMessage::SC_LoginAns* packet)
{
    Player *mine = new Player(packet->sessionid(), packet->nickname()->c_str(), 0);

    EntityManager::instance().players.Add(packet->sessionid(), mine);
    EntityManager::instance().players.SetMySession(packet->sessionid());

    std::cout << "로그인 성공: " << packet->nickname()->c_str() << std::endl;
    std::cout << "월드 진입 시작" << std::endl;

    NetworkMessage::CS_WorldJoinReq_FBS req;
    req.gate = 0;
    Send(NetworkMessage::ClientPackets::ClientPackets_CS_WorldJoinReq, &req);

}

void WebSocketClient::HANDLE_PACKET_SC_AccountInfo(const NetworkMessage::SC_AccountInfo* packet)
{
}

void WebSocketClient::HANDLE_PACKET_SC_WorldEnterNotify(const NetworkMessage::SC_WorldEnterNotify* packet)
{
    EntityManager::instance().OnWorldEnterNotify(packet->zoneid());
}

void WebSocketClient::HANDLE_PACKET_SC_CreatePlayer(const NetworkMessage::SC_CreatePlayer* packet)
{
    auto playermanager = &EntityManager::instance().players;
    Player* player = new Player(packet->player()->sessionid(), packet->player()->name()->c_str(), packet->player()->realm());
    player->UpdatePosition(packet->player()->head(),
        Vector3{ packet->player()->position()->x(),packet->player()->position()->y(),packet->player()->position()->z() });

    playermanager->Lock();
    playermanager->Add(packet->player()->sessionid(), player);
    playermanager->UnLock();
    std::cout << " - 플레이어 입장 : " << packet->player()->name()->c_str() << std::endl;
}

void WebSocketClient::HANDLE_PACKET_SC_CreatePlayers(const NetworkMessage::SC_CreatePlayers* packet)
{
    auto playerArray = packet->player();
    auto playermanager = &EntityManager::instance().players;
    if (playerArray == nullptr)
        return;

    playermanager->Lock();
    std::cout << "  - 이미 있는 플레이어 : " << playerArray->size() << "\n";
    for (auto pinfo : *playerArray)
    {
        auto player = playermanager->getPlayer(pinfo->sessionid());
        if (player != nullptr )
        {
            if (player->GetName().compare(pinfo->name()->c_str()) != 0)
            {
                playermanager->Remove(pinfo->sessionid());
                player = nullptr;
            }
        } 

        if (player == nullptr)
        {
            player = new Player(pinfo->sessionid(), pinfo->name()->c_str(), pinfo->realm());
            playermanager->Add(pinfo->sessionid(), player);
        }
        
        player->UpdatePosition(pinfo->head(), Vector3{ pinfo->position()->x(),pinfo->position()->y(),pinfo->position()->z() });
        
        std::cout << "     플레이어 : " << pinfo->name()->c_str() << "\n";
    }
    playermanager->UnLock();

    if (EntityManager::instance().zoneLoaded.load() == false)
    {
        EntityManager::instance().zoneLoaded.store(true);
    }
}

void WebSocketClient::HANDLE_PACKET_SC_UpdatePosition(const NetworkMessage::SC_UpdatePosition* packet)
{
    EntityManager::instance().players.Lock();
    auto player = EntityManager::instance().players.getPlayer(packet->sessionid());
    if (player != nullptr)
    {
        player->UpdatePosition(packet->head(), Vector3{ packet->position()->x(),packet->position()->y(),packet->position()->z() });
    }
    EntityManager::instance().players.UnLock();
}

std::shared_ptr<NetworkMessage::Vector3> WebSocketClient::Vector3Convert(Vector3 pos)
{
    return std::make_shared<NetworkMessage::Vector3>(pos.x, pos.y, pos.z);
}