#ifndef ASYN_SERVER_SOCKET_H
#define ASYN_SERVER_SOCKET_H

// Windows�׽�����Ҫ��
#include "winsock2.h"
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <string>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <iostream>

class tcpthread
{
public:
	void Main()
	{
		char buf[1024] = { 0 };//������Ϣ����󳤶ȣ���λbuf
		while (true)
		{
			int recvlen = recv(client, buf, sizeof(buf) - 1, 0);//windowsû��read������linux����
			if (recvlen <= 0)break;//û���յ�
			if (strstr(buf, "q") != NULL)//��q�˳�
			{
				char re[] = "quit success!!!\n";
				send(client, re, strlen(re) + 1, 0);//��1����Ϊ����\0
				break;
			}
			int sendlen = send(client, "ok\n", 3, 0);//linux����ʹ��write
			std::cout << "receive:" << buf << std::endl;
			//len�ǽ������ݵ�ʵ�ʴ�С��len<=buf���ȣ�������1024��
		}


		closesocket(client);//�ر�����

	}
	int client = 0;
};


class AsynServerSocket
{
public:
	AsynServerSocket() = default;
	virtual~AsynServerSocket() = default;

public:
	// ���÷����socket��ip�Ͷ˿�
	void SetServerSocketIPAndPort(const char* port);

	// ���������ӷ����Socket
	bool CreateServerSocket();

	// �ر��׽���
	void CloseSocket();
private:
	void ServerProcessThreadFunction();

	void SingleSocketReciveThreadFunction(int socket);
private:
	SOCKADDR_IN m_ServerAddress;
	SOCKET m_ServerSocket;

	std::shared_ptr<std::thread> m_ServerRunThreadPtr;
	std::vector<std::shared_ptr<std::thread>> m_SingleSocketRecvThreadVector;

	std::mutex m_SingleSocketMutex;
};

#endif // !ASYN_SERVER_SOCKET_H
