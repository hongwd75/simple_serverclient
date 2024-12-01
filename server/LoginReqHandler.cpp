#include "LoginReqHandler.h"
#include "ServerMain.h"

LockManager LoginReqHandler::lockman;

void LoginReqHandler::HandlePacket( Client* user, const::flatbuffers::Vector<uint8_t>* recvdata)
{
	auto packet = flatbuffers::GetRoot<NetworkMessage::CS_LoginReq>(recvdata->data());
	if (packet != nullptr)
	{

		try {
			std::string _id = packet->id()->str();
			std::string _pwd = packet->pwd()->str();
			ServerMain::instance()->TaskMan()->addFunctionTask(
				[this, user, id = _id, pwd = _pwd]()
				{
					check_login(user, id, pwd);
				}
			);
		}
		catch (const std::exception& e) {
			std::cerr << "Error adding task: " << e.what() << std::endl;
		}
	}
}

/* ----------------------------------------------------------------------------------
   내부 로직
   ---------------------------------------------------------------------------------- */
void LoginReqHandler::sendLoginAns(Client* user,int errorcode)
{
	NetworkMessage::SC_LoginAns_FBS req;
	req.errorcode = errorcode;
	req.nickname = user->GetAccount()->AccountName;
	req.sessionid = user->GetSessionID();

	auto socketInstance = ServerMain::instance()->SocketMan()->GetSocketpp();
	auto connectSessions = ServerMain::instance()->SocketMan()->GetClientManager();
	user->Send(NetworkMessage::ServerPackets::ServerPackets_SC_LoginAns, &req);
}

void LoginReqHandler::check_login(Client* user, const std::string& id, const std::string& pwd)
{
	lockman.EnterLock(id);

	// 로그인 체크 로직 (여기서는 간단히 하드코딩된 체크)
	uint64_t DB_uid = atoi(pwd.c_str());
	auto socketInstance = ServerMain::instance()->SocketMan()->GetSocketpp();
	auto connectSessions = ServerMain::instance()->SocketMan()->GetClientManager();

	auto lockedhdl = user->GetSocket().lock();
	if (DB_uid > 0)
	{
		connectSessions->Lock();
		auto newuser = connectSessions->GetClientByUID(DB_uid);
		if (newuser != nullptr)
		{
			// 이미 접속중인 상태가 있다.
			if (newuser->GetSocket().expired() == true)
			{
				// 재접속 가능
				connectSessions->ChangeSocketSession(DB_uid, user->GetSocket());

				// 재접속에 필요한 정보 다시 전송해줘야 한다.
				if (newuser->GetPlayerState() == Enums::ClientState::Room)
				{
					// 메시지 전송
					sendLoginAns(newuser, 0);
				}
			}
			else
			{

				// 킥!
				socketInstance->close(user->GetSocket(), websocketpp::close::status::normal, "dual login");
				std::cout << "    * 중복로그인 : " << id.c_str() << std::endl;

				lockman.ExitLock(id);
				connectSessions->Unlock();
				return;
			}
			
		}
		else
		{
			if (user != nullptr)
			{
				Account connectuserinfo;
				connectuserinfo.UID = DB_uid;
				connectuserinfo.AccountName = id;
				connectuserinfo.realm = (DB_uid % 3) + 1;
				user->SetAccount(&connectuserinfo);
				connectSessions->AddLoginedUser(user);
				user->SetPlayerState(Enums::ClientState::Lobby);
				// 메시지 전송
				sendLoginAns(user, 0);
			}
		}
		connectSessions->Unlock();
	}
	else
	{
		// 접속 끊어
		socketInstance->close(user->GetSocket(), websocketpp::close::status::normal, "login failed");
	}

	lockman.ExitLock(id);
}
