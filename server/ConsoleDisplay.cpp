#include "ConsoleDisplay.h"
#include <iostream>
#include <vector>
#include <cstdarg>
#include <mutex>

std::mutex log_mutex;

void ConsoleDisplay::WriteFormat(TYPE logtype, const char* format, ...)
{
	std::lock_guard<std::mutex> lock(log_mutex);
	// 가변 인자 리스트 초기화
	va_list args;
	va_start(args, format);

	// 포맷된 메시지 버퍼
	int size = std::vsnprintf(nullptr, 0, format, args) + 1;
	std::vector<char> buffer(size);
	vsnprintf(buffer.data(), buffer.size(), format, args); // format과 args를 사용해 포맷된 문자열 작성
	va_end(args);

	Write(logtype, buffer.data());
}

void ConsoleDisplay::Write(TYPE logtype, const char* message)
{
	std::string ansi = "";

	switch (logtype)
	{
		case TYPE::Error:
			ansi = "\033[1;31m";
		break;
		case TYPE::Important:
			ansi = "\033[1;32m";
		break;
		case TYPE::Warring:
			ansi = "\033[1;33m";
		break;
		case TYPE::Success:
			ansi = "\033[1;34m";
			break;
		case TYPE::ServerState:
			ansi = "\033[1;35m";
			break;
		default:
			ansi = "";
		break;

	}

	std::cout << ansi << message << "\033[0m" << std::endl;
}

void ConsoleDisplay::LogMessage(TYPE logtype, const std::string& message)
{
	std::string ansi = "";

	switch (logtype)
	{
	case TYPE::Error:
		ansi = "\033[1;31m";
		break;
	case TYPE::Important:
		ansi = "\033[1;32m";
		break;
	case TYPE::Warring:
		ansi = "\033[1;33m";
		break;
	case TYPE::Success:
		ansi = "\033[1;34m";
		break;
	case TYPE::ServerState:
		ansi = "\033[1;35m";
		break;
	default:
		ansi = "";
		break;

	}
	std::cout << ansi << message << std::endl;
}