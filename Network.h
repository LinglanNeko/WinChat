#pragma once
#include <winsock2.h>
#include <string>
#include <ctime>
#include "NetProtocol.h"

//初始化网络环境
bool InitNetwork();

//清理网络环境
void CleanNetwork();

//发送函数
void SendNetPacket(SOCKET clientSocket, MessageType type, const std::string& username, const std::string& content);

//获取系统时间
std::string GetCurrentTimePrefix();