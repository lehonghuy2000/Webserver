#include "WebServer.h"
int main()
{
	WebServer webServer("0.0.0.0", 80);
	if (webServer.Init() != 0)
		return 0;

	webServer.Running();

	system("pause");
}
