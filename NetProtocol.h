#pragma once

// 协议头文件

// 消息类型枚举
enum MessageType
{
	MSG_LOGIN = 1,
	MSG_CHAT = 2,
	MSG_PRIVATE =3
};

// 统一的网络数据包结构
#pragma pack(push, 1)

struct NetPacket
{
	int type;
	char username[32];
	char content[1024];
	char time;
};

#pragma pack(pop)