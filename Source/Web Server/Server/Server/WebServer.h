#pragma once
#include "TCP_Listening.h"
#include <string>
#include <istream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>
#include <iterator>
#include "dirent.h"
#include <sys/types.h>
#include <vector>
#include <windows.h>
#include <atlstr.h>
#include <iomanip>
using namespace std;
class WebServer : public TCP_Listening
{
public:
	WebServer();
	WebServer(const char* ipAddress, int port);
protected:

	virtual void ClientConnected(int CSocket);
	virtual void ClientDisconnected(int CSocket);
	virtual void MessageReceived(int CSocket, const char* message, int length);
};

