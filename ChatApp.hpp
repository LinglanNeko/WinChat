/* ChatApp */

#pragma once
#include <iostream>
#include "chat_client.hpp"
#include "chat_server.hpp"


class ChatApp
{
public:
	void run()
	{
		showMenu();

		if (m_role == 1)
		{
			m_server.start();
		}
		else if (m_role == 2)
		{
			m_client.start();
		}
	}

private:
	void showMenu()
	{
		std::cout << "1.成为主机" << "\n2.成为客机" << std::endl;
		std::cin >> m_role;
	}
	int m_role;
	
	chat_server m_server;
	chat_client m_client;
};