#ifndef ASYN_CLIENT_SOCKET_H
#define ASYN_CLIENT_SOCKET_H

// Windows�׽�����Ҫ��
#include "winsock2.h"
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <memory>

class AsynClientSocket
{
public:
	typedef std::shared_ptr<AsynClientSocket> ptr;
	AsynClientSocket() = default;
	virtual~AsynClientSocket() = default;

public:
	// ����Զ�̷����socket��ip�Ͷ˿�
	void SetRemoteServerSocketIPAndPort(const char* ip, const char* port);

	// ���������ӵ�Զ�̷�����
	bool CreateAndConnectToRemoteServer();

	// ��������
	bool SendDataToRemoteServer(const char* data,int length);

	// �ر��׽���
	void CloseClient();
private:
	SOCKADDR_IN m_RemoteServerAddress;
	SOCKET m_Socket;
};

#endif // !ASYN_CLIENT_SOCKET_H
