#include "Client.h"
#include "WebSocketServer.h"
#include "PacketHandlerFactory.h"

#define HANDLE_PACKET(packetName, handlerCode) \
    case NetworkMessage::ClientPackets::ClientPackets_##packetName: { \
        auto packet = flatbuffers::GetRoot<NetworkMessage::packetName>(packetData->data()); \
        handlerCode(packet); \
        break; \
    }

Client::Client(websocketpp::connection_hdl hdl, int sessionid, WebSocketServer * svr):connection(hdl),_svr(svr), SessionID(sessionid)
{
}

websocketpp::connection_hdl Client::GetSocket()
{
	return connection;
}

void Client::SetSocket(websocketpp::connection_hdl sock)
{
	connection = sock;
}

void Client::SetAccount(Account* info)
{
	m_account = *info;
}

void Client::SetPlayerState(Enums::ClientState st)
{
	clientState = st;
}

Enums::ClientState Client::GetPlayerState()
{
	return clientState;
}

Account* Client::GetAccount()
{
	return &m_account;
}

void Client::OnRecive(const uint16_t packetType, const ::flatbuffers::Vector<uint8_t>* packetData)
{
	auto packetprocessor = PacketHandlerFactory::Instance().GetHandler((NetworkMessage::ClientPackets)packetType);
	if (packetprocessor != nullptr)
	{
		packetprocessor->HandlePacket(this, packetData);
	}
	//auto packetWarpper = NetworkMessage::GetPacketWrapper(buffer);
	//auto packetType = packetWarpper->type();
	//auto packetData = packetWarpper->packet();
	//switch (packetType)
	//{
	//	HANDLE_PACKET(CS_LoginReq, LoginReq)
	//	HANDLE_PACKET(CS_UpdatePosition, UpdatePosition)
	//}
}


void Client::SendInternal(const char* data, size_t size)
{
	try
	{
		if (auto conn = connection.lock())
		{
			_svr->Send(conn, data, size);
		}
	}
	catch (const std::exception&)
	{

	}
}

void Client::Log(const char* msg)
{
	std::cout << msg << std::endl;
}
