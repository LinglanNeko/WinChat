#pragma once
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <string>

#include "Network.h"

#define DEFAULT_SERVER "服务器"
#define DEFAULT_CLIENT "客机"
#define PREFIX "[系统]"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#pragma comment(lib, "Ws2_32.lib")

class chat_server
{
public:
	void start()
	{
		run();
	}

private:

	//发送信息函数
	void send_handler(SOCKET ClientSocket)
	{
		std::string server_reply;
		while (true)
		{
			std::cin.clear();
			std::getline(std::cin, server_reply);

			if (server_reply == "exit")
			{
				std::cout << GetCurrentTimePrefix() << PREFIX << "正在退出聊天..." << std::endl;
				break;
			}
			
			if (!server_reply.empty())
			{
				SendNetPacket(ClientSocket, MSG_CHAT, DEFAULT_SERVER, server_reply);
				server_reply.clear();
			}
		}
		close(ClientSocket);
	}

	//接收消息函数
	void recv_handler(SOCKET ClientSocket)
	{
		NetPacket packet;

		while (true)
		{
			int res = recv(ClientSocket, (char*)&packet, sizeof(NetPacket), 0);

			if (res > 0)
			{
				switch (packet.type)
				{
				case MSG_LOGIN:
					std::cout << GetCurrentTimePrefix() << PREFIX << packet.username << "成功登录系统!" << std::endl;
					break;
				case MSG_CHAT:
					std::cout << "[" << packet.username << "]" << packet.content << "\n";
					break;
				}
			}
			else if(res == 0)
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
		close(ClientSocket);
	}

		void run()
		{
			int iResult;

			struct addrinfo* result = NULL, * ptr = NULL, hints = { 0 };

			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			//解析服务器将使用的本地地址和端口
			iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
			if (iResult != 0)
			{
				std::cout << GetCurrentTimePrefix() << PREFIX << "getaddrinfo 失败:" << iResult << std::endl;
			}

			SOCKET ListenSocket = INVALID_SOCKET;

			//为服务器创建一个用于监听客户端连接的 SOCKET
			ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (ListenSocket == INVALID_SOCKET)
			{
				std::cout << GetCurrentTimePrefix() << PREFIX << "创建 Socket 失败:" << WSAGetLastError() << std::endl;
				freeaddrinfo(result);
				return;
			}

			//绑定套接字
			iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR)
			{
				std::cout << GetCurrentTimePrefix() << PREFIX << "bind 绑定失败:" << WSAGetLastError() << std::endl;
				freeaddrinfo(result);
				return;
			}

			freeaddrinfo(result);

			//侦听套接字
			if (listen(ListenSocket, 1) == SOCKET_ERROR)
			{
				std::cout << "listen 监听失败: " << WSAGetLastError() << std::endl;
				return;
			}

			std::cout << GetCurrentTimePrefix() << PREFIX << "服务器已成功拉起，正在等待客机连接..." << std::endl;

			std::cin.clear();

			//接受连接
			SOCKET ClientSocket;
			ClientSocket = accept(ListenSocket, NULL, NULL);

			if (ClientSocket == INVALID_SOCKET)
			{
				std::cout << GetCurrentTimePrefix() << PREFIX << "accept 接收连接失败:" << WSAGetLastError() << std::endl;
				return;
			}

			std::cout << GetCurrentTimePrefix() << PREFIX << "有客机成功连入！开始进入聊天双工通道。" << std::endl;

			//不在需要服务器套接字
			closesocket(ListenSocket);

			//在服务器上接收和发送数据
			char recvbuf[DEFAULT_BUFLEN];
			int recvbuflen = DEFAULT_BUFLEN;
			std::string server_reply;	//用于存储服务器需要发送的信息

			
			std::thread sendThread(&chat_server::send_handler, this, ClientSocket);
			sendThread.detach();

			recv_handler(ClientSocket);

	}

	//断开服务器连接
	void close(SOCKET& ClientSocket)
	{
		if (ClientSocket != INVALID_SOCKET)
		{
			//优雅的先关闭发送端，允许把缓冲区剩下的数据发完
			shutdown(ClientSocket, SD_SEND);
			closesocket(ClientSocket);
			ClientSocket = INVALID_SOCKET;
			std::cout << GetCurrentTimePrefix() << PREFIX << "与客机的网络资源已安全回收。" << std::endl;
		}
	}
};