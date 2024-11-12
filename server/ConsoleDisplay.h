#pragma once
class ConsoleDisplay
{

public:
	enum class TYPE
	{
		Normal = 0,
		Success,
		Important,
		Warring,
		ServerState,
		Error
	};

public:
	
	static void WriteFormat(TYPE logtype, const char* format, ...);
	static void Write(TYPE logtype, const char* message);
};

