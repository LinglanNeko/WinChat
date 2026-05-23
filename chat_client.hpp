#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <thread>
#include <string>

#include "Network.h"

#define DEFAULT_SERVER "服务器"
#define DEFAULT_CLIENT "客机"
#define PREFIX "[系统]"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#pragma comment(lib, "Ws2_32.lib")


//聊天客户端 Client
class chat_client
{
public:
	void start()
	{
		run();
	}

private:

	//发送信息函数
	void send_handler(SOCKET CoonectSocket)
	{
		std::string sendbuf;

		while (true)
		{
			std::cin.clear();
			std::getline(std::cin, sendbuf);

			if (sendbuf == "exit")
			{
				std::cout << GetCurrentTimePrefix()<< PREFIX << "正在退出聊天..." << std::endl;
				break;
			}
			
			if (!sendbuf.empty())
			{
				SendNetPacket(CoonectSocket, MSG_CHAT, DEFAULT_CLIENT, sendbuf);
				sendbuf.clear();
			}
		}
		close(CoonectSocket);
	}

	//接收信息函数
	void recv_handler(SOCKET CoonectSocket)
	{
		NetPacket packet;

		while (true)
		{
			int res = recv(CoonectSocket, (char*)&packet, sizeof(NetPacket), 0);

			if (res > 0)
			{
				switch (packet.type)
				{
				case MSG_LOGIN:
					std::cout << GetCurrentTimePrefix << PREFIX << packet.username << "成功登录系统!" << std::endl;
					break;
				case MSG_CHAT:
					std::cout << "[" << packet.username << "]" << packet.content << "\n";
					break;
				}
			}
			else if (res == 0)
			{
				std::cout << "\n" << GetCurrentTimePrefix() << PREFIX << "对方主动断开连接，聊天结束。" << std::endl;
				break;
			}
			else
			{
				std::cout << "\n" << GetCurrentTimePrefix() << PREFIX << "对方连接异常中断。" << std::endl;
				break;
			}
		}
	}

	void run()
	{
		int iResult;

		struct addrinfo* result = NULL, * ptr = NULL, hints = { 0 };
	
		std::string ip;
		std::cout << GetCurrentTimePrefix() << PREFIX << "请输入服务器的 IP 地址:" << std::endl;
		std::cin >> ip;

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		iResult = getaddrinfo(ip.c_str(), DEFAULT_PORT, &hints, &result);
		if (iResult != 0)
		{
			std::cout << GetCurrentTimePrefix() << PREFIX << "getaddrinfo 解析失败. 错误码:" << iResult << std::endl;
			return;
		}

		SOCKET CoonectSocket = INVALID_SOCKET;

		ptr = result;

		CoonectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (CoonectSocket == INVALID_SOCKET)
		{
			std::cout << GetCurrentTimePrefix() << PREFIX << "socket 创建失败. 错误码:" << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			return;
		}

		//连接至服务器
		iResult = connect(CoonectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(CoonectSocket);
			CoonectSocket = INVALID_SOCKET;
		}
		freeaddrinfo(result);

		if (CoonectSocket == INVALID_SOCKET)
		{
			std::cout << GetCurrentTimePrefix() << PREFIX << "无法连接至服务器! 请检查服务器是否开启或 IP 是否正确!" << std::endl;
			return;
		}

		std::cout << GetCurrentTimePrefix() << PREFIX << "成功连接上服务器！现在可以开始聊天了。" << std::endl;

		int recvbuflen = DEFAULT_BUFLEN;



		std::thread sendThread(&chat_client::send_handler, this, CoonectSocket);
		sendThread.detach();

		recv_handler(CoonectSocket);
	
	}


	//关闭连接
	void close(SOCKET& ConectSocket)
	{
		//close
		if (ConectSocket != INVALID_SOCKET)
		{
			shutdown(ConectSocket, SD_SEND);
			closesocket(ConectSocket);
			ConectSocket = INVALID_SOCKET;
			std::cout << GetCurrentTimePrefix() << PREFIX << "与服务器的网络资源已安全回收。" << std::endl;
		}
	}


};