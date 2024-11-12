#pragma once
namespace Enums
{ 
	enum class ClientState
	{
		NotConnected = 0,
		Connecting,
		Login,
		Lobby,
		Room,
		Logout = 100
	};

	enum class LoginResult
	{
		none = 0, // 로그인 성공
		Relog,
		LgoinError = 100,
		DualLogin1,
		DualLogin2,
	};

	enum class ConnectState
	{
		Green,
		Yellow,
		Red,
		Disconnected,
	};
}

namespace Structs
{
	struct Vector3
	{
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
	};
}