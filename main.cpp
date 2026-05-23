
/* 程序的主入口 */

#include "ChatApp.hpp"
#include "Network.h"

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	InitNetwork();

	ChatApp app;

	app.run();

	system("pause");

	return 0;
}