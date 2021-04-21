#include "TCP_Listening.h"

TCP_Listening::TCP_Listening()
{

}
TCP_Listening::TCP_Listening(const char* ipAddress, int port)
{ 
	m_ipAddress = ipAddress;
	m_port = port;
}
int TCP_Listening::Init()
{
	// Khởi tạo winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		return wsOk;
	}

	// Tạo socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}
	// Liên kết địa chỉ ip và port vào socket
	sockaddr_in adr;
	adr.sin_family = AF_INET;
	adr.sin_port = htons(m_port);
	inet_pton(AF_INET, m_ipAddress, &adr.sin_addr);
	if (bind(m_socket, (sockaddr*)&adr, sizeof(adr)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	// Socket bắt đầu lắng nghe
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}
	// Tạo mảng chứa socket và khởi tạo
	FD_ZERO(&m_master);
	FD_SET(m_socket, &m_master);
	return 0;
}

int TCP_Listening::Running()
{
	bool running = true;
	while (running)
	{
		fd_set copy = m_master;

		// Tính số lượng Client đang kết nối
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Duyệt toàn bộ socket và thực hiện gửi 
		for (int i = 0; i < socketCount; i++)
		{
			// Lấy socket cần kết nối ra
			SOCKET sock = copy.fd_array[i];
			if (sock == m_socket)
			{
				// Chấp nhận một kết nối
				SOCKET client = accept(m_socket, nullptr, nullptr);

				// Thêm socket vào mảng các socket
				FD_SET(client, &m_master);

				ClientConnected(client);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Nhận thông điệp
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					ClientDisconnected(sock);
					closesocket(sock);
					FD_CLR(sock, &m_master);
				}
				else
				{
					MessageReceived(sock, buf, bytesIn);
				}
			}
		}
	}

	// Xoá socket khỏi mảng
	FD_CLR(m_socket, &m_master);
	closesocket(m_socket);
	// Nếu còn socket kết nối thì xoá hết để ngăn các truy cập không cho phép
	while (m_master.fd_count > 0)
	{
		SOCKET sock = m_master.fd_array[0];
		FD_CLR(sock, &m_master);
		closesocket(sock);
	}
	WSACleanup();
	return 0;
}


void TCP_Listening::sendToClient(int CSocket, const char* message, int length)
{
	send(CSocket, message, length, 0);
}

void TCP_Listening::sendAllClients(int SSocket, const char* message, int length)
{
	for (int i = 0; i < m_master.fd_count; i++)
	{
		SOCKET outSock = m_master.fd_array[i];
		if (outSock != m_socket && outSock != SSocket)
		{
			sendToClient(outSock, message, length);
		}
	}
}

void TCP_Listening::ClientConnected(int CSocket)
{

}

void TCP_Listening::ClientDisconnected(int CSocket)
{

}

void TCP_Listening::MessageReceived(int CSocket, const char* message, int length)
{

}