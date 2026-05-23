#include "Network.h"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define PREFIX "[Network]"

//初始化网络环境
bool InitNetwork()
{
	WSADATA wsaData;

	return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

//清理网络环境
void CleanNetwork()
{
	WSACleanup();
}

//发送函数
void SendNetPacket(SOCKET clientSocket, MessageType type, const std::string& username, const std::string& content,const std::string& time)
{
	NetPacket packet;

	memset(&packet, 0, sizeof(NetPacket));

	packet.type = type;
	strncpy_s(packet.username, username.c_str(), _TRUNCATE);
	strncpy_s(packet.content, content.c_str(), _TRUNCATE);
	strncpy_s(packet.time,time.c_str(),_TRUNCATE);

	//发送
	int bytesSent = send(clientSocket, (char*)&packet, sizeof(NetPacket), 0);
	if (bytesSent == SOCKET_ERROR)
	{
		std::cout << GetCurrentTimePrefix() << PREFIX << "数据包发送失败" << WSAGetLastError() << std::endl;
	}
}

//获取系统时间
std::string GetCurrentTimePrefix()
{
	std::time_t time_now = std::time(nullptr);
	std::tm tm_local;
	localtime_s(&tm_local, &time_now);

	char buf[16];

	std::strftime(buf, sizeof(buf), "[%H:%M:%S]", &tm_local);

	return std::string(buf);
}