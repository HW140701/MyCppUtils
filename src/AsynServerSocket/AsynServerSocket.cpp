#include "AsynServerSocket.h"

#include <sstream>

#include <iostream>

AsynServerSocket::AsynServerSocket()
{
	m_IsCloseServer = false;
}

void AsynServerSocket::SetServerPort(const char* port)
{
	// 设置当前服务器地址
	SOCKADDR_IN		serverAddress;

	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1", (void*)&serverAddress.sin_addr.S_un.S_addr);

	// 端口字符型转int型
	std::istringstream intStr(port);
	int tempPort;
	intStr >> tempPort;

	serverAddress.sin_port = htons(tempPort);

	m_ServerAddress = serverAddress;
}

bool AsynServerSocket::CreateServerSocket()
{
	int result = -1;

	// 初始化Windows套接字
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		return false;
	}
	std::cout << "初始化Windows套接字成功" << std::endl;

	// 创建套接字
	m_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_ServerSocket)
	{
		WSACleanup();
		return false;
	}
	std::cout << "创建服务器套接字成功" << std::endl;

	// 设置套接字为非阻塞模式
	//unsigned long ul = 1;
	//result = ioctlsocket(m_ServerSocket, FIONBIO, (unsigned long*)&ul);
	//if (SOCKET_ERROR == result)
	//{
	//	WSACleanup();
	//	return false;
	//}


	// 绑定套接字
	result = bind(m_ServerSocket, (LPSOCKADDR)&m_ServerAddress, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == result)
	{
		closesocket(m_ServerSocket);
		WSACleanup();
		return false;
	}

	std::cout << "绑定服务器套接字成功" << std::endl;

	// 开始监听
	result = listen(m_ServerSocket, 5);
	if (SOCKET_ERROR == result)
	{
		closesocket(m_ServerSocket);
		WSACleanup();
		return false;
	}

	std::cout << "服务器套接字开始监听" << std::endl;

	// 开始接受客户端请求
	m_ServerMainThreadPtr = std::make_shared<std::thread>(&AsynServerSocket::ServerProcessThreadFunction,this);

	return true;
}

void AsynServerSocket::CloseServer()
{
	// 关闭服务器，让分离子线程退出
	m_IsCloseServer = true;

	// 关闭套接字和清理资源
	closesocket(m_ServerSocket);
	WSACleanup();

	m_ServerMainThreadPtr->join();
}

void AsynServerSocket::ServerProcessThreadFunction()
{
	while (true)
	{
		if (m_IsCloseServer)
		{
			break;
		}

		SOCKET clientSocket;
		SOCKADDR_IN addrClient;
		int len = sizeof(SOCKADDR);

		clientSocket = accept(m_ServerSocket, (SOCKADDR*)&addrClient, &len);

		std::cout << "一个客户端已连接到服务器，socket是：" << clientSocket << std::endl;

		std::shared_ptr<std::thread> singleSocketRecvThread = std::make_shared<std::thread>(&AsynServerSocket::SingleSocketReciveThreadFunction, this, clientSocket);
		singleSocketRecvThread->detach();
	}

	std::cout << "服务器运行子线程正确退出" << std::endl;
}

void AsynServerSocket::SingleSocketReciveThreadFunction(int socket)
{
	int clientSocket = socket;

	while (true)
	{
		char recvBuf[1024] = {0};

		int recvlen = recv(clientSocket, recvBuf, sizeof(recvBuf) - 1, 0);

		std::cout << "收到客户端消息:" << recvBuf << std::endl;

		// 客户端已关闭连接
		if (recvlen == 0)
		{
			std::cout << "socket连接关闭" << std::endl;

			closesocket(socket);

			break;
		}
		// 出错
		else if (recvlen == -1)
		{

			break;
		}	
	}

	std::cout << "服务器单个接收数据子线程正确退出" << std::endl;
}
