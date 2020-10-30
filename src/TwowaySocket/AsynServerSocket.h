#ifndef ASYN_SERVER_SOCKET_H
#define ASYN_SERVER_SOCKET_H


#include "winsock2.h"
#include <WS2tcpip.h>


// Windows�׽�����Ҫ��
//#include "winsock2.h"
//#include <WS2tcpip.h>
//#pragma comment(lib, "ws2_32.lib")

#include <string>
#include <thread>
#include <memory>
#include <map>
#include <mutex>
#include <iostream>
#include <atomic>
#include <functional>

//typedef std::function<bool(const std::string& message)> ServerMessageCallbackFunc;

class AsynServerSocket
{
public:
	typedef std::shared_ptr<AsynServerSocket> ptr;
	AsynServerSocket();
	virtual~AsynServerSocket() = default;

public:
	// ���÷����socket��ip�Ͷ˿�
	void SetServerPort(const char* port);

	// ���������ӷ����Socket
	bool CreateServerSocket();

	// �رշ�����
	void CloseServer();

	std::string GetMessage();
private:
	void ServerProcessThreadFunction();

	void SingleSocketReciveThreadFunction(int socket, int threadIndex);
private:
	SOCKADDR_IN m_ServerAddress;
	SOCKET m_ServerSocket;

	std::shared_ptr<std::thread> m_ServerMainThreadPtr;
	std::map<int,std::shared_ptr<std::thread> > m_SingleSocketRecvThreadMap;

	std::mutex m_SingleSocketMutex;

	std::atomic<bool> m_IsCloseServer;
};

#endif // !ASYN_SERVER_SOCKET_H
