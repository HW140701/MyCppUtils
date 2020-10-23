#include "AsynServerSocket.h"

#include <sstream>

#include <iostream>

void AsynServerSocket::SetServerSocketIPAndPort(const char* port)
{
	// ���õ�ǰ��������ַ
	SOCKADDR_IN		serverAddress;

	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1", (void*)&serverAddress.sin_addr.S_un.S_addr);

	// �˿��ַ���תint��
	std::istringstream intStr(port);
	int tempPort;
	intStr >> tempPort;

	serverAddress.sin_port = htons(tempPort);

	m_ServerAddress = serverAddress;
}

bool AsynServerSocket::CreateServerSocket()
{
	int result = -1;

	// ��ʼ��Windows�׽���
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		return false;
	}
	std::cout << "��ʼ��Windows�׽��ֳɹ�" << std::endl;

	// �����׽���
	m_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_ServerSocket)
	{
		WSACleanup();
		return false;
	}
	std::cout << "�����������׽��ֳɹ�" << std::endl;

	// �����׽���Ϊ������ģʽ
	//unsigned long ul = 1;
	//result = ioctlsocket(m_ServerSocket, FIONBIO, (unsigned long*)&ul);
	//if (SOCKET_ERROR == result)
	//{
	//	WSACleanup();
	//	return false;
	//}


	// ���׽���
	result = bind(m_ServerSocket, (LPSOCKADDR)&m_ServerAddress, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == result)
	{
		closesocket(m_ServerSocket);
		WSACleanup();
		return false;
	}

	std::cout << "�󶨷������׽��ֳɹ�" << std::endl;

	// ��ʼ����
	result = listen(m_ServerSocket, 5);
	if (SOCKET_ERROR == result)
	{
		closesocket(m_ServerSocket);
		WSACleanup();
		return false;
	}

	std::cout << "�������׽��ֿ�ʼ����" << std::endl;

	// ��ʼ���ܿͻ�������
	m_ServerRunThreadPtr = std::make_shared<std::thread>(&AsynServerSocket::ServerProcessThreadFunction,this);

	return true;
}

void AsynServerSocket::CloseSocket()
{
	// �ȴ����߳�����
	m_ServerRunThreadPtr->join();

	closesocket(m_ServerSocket);
	WSACleanup();
}

void AsynServerSocket::ServerProcessThreadFunction()
{
	while (true)
	{
		SOCKET clientSocket;
		SOCKADDR_IN addrClient;
		int len = sizeof(SOCKADDR);

		clientSocket = accept(m_ServerSocket, (SOCKADDR*)&addrClient, &len);

		std::cout << "һ���ͻ��������ӵ���������socket�ǣ�" << clientSocket << std::endl;

		std::shared_ptr<std::thread> singleSocket = std::make_shared<std::thread>(&AsynServerSocket::SingleSocketReciveThreadFunction, this, clientSocket);
		singleSocket->detach();
	}
}

void AsynServerSocket::SingleSocketReciveThreadFunction(int socket)
{
	int clientSocket = socket;

	while (true)
	{
		char recvBuf[1024] = {0};

		int recvlen = recv(clientSocket, recvBuf, sizeof(recvBuf) - 1, 0);

		std::cout << "�յ��ͻ�����Ϣ:" << recvBuf << std::endl;
	}
}
