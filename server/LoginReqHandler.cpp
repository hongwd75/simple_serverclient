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
   ���� ����
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

	// �α��� üũ ���� (���⼭�� ������ �ϵ��ڵ��� üũ)
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
			// �̹� �������� ���°� �ִ�.
			if (newuser->GetSocket().expired() == true)
			{
				// ������ ����
				connectSessions->ChangeSocketSession(DB_uid, user->GetSocket());

				// �����ӿ� �ʿ��� ���� �ٽ� ��������� �Ѵ�.
				if (user->GetPlayerState() == Enums::ClientState::Room)
				{

				}
			}
			else
			{

				// ű!
				socketInstance->close(user->GetSocket(), websocketpp::close::status::normal, "dual login");
				std::cout << "    * �ߺ��α��� : " << id.c_str() << std::endl;

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
				// �޽��� ����
				sendLoginAns(user, 0);
			}
		}
		connectSessions->Unlock();
	}
	else
	{
		// ���� ����
		socketInstance->close(user->GetSocket(), websocketpp::close::status::normal, "login failed");
	}

	lockman.ExitLock(id);
}
