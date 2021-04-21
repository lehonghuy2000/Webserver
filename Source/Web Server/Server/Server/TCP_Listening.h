#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

class TCP_Listening
{

public:
	TCP_Listening();
	TCP_Listening(const char* ipAddress, int port);
	int Init();
	int Running();

protected:

	virtual void ClientConnected(int CSocket);
	virtual void ClientDisconnected(int CSocket);
	virtual void MessageReceived(int CSocket, const char* message, int length);
	void sendToClient(int CSocket, const char* message, int length);
	void sendAllClients(int sendingClient, const char* message, int length);
	const char* m_ipAddress;	// Địa chỉ IP Socket
	int	m_port;			// Cổng của Server
	int	m_socket;		// Socket
	fd_set m_master;		// Mảng socket
};

