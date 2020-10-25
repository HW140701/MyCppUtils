#ifndef _TWOWAY_SOCKET_H
#define _TWOWAY_SOCKET_H

#include "AsynClientSocket.h"
#include "AsynServerSocket.h"

#include <vector>
#include <memory>
#include <string>
#include <functional>

typedef std::function<bool(const std::string& message)> ServerMessageCallbackFunc;

class TwowaySocket
{
public:
	TwowaySocket();
	virtual~TwowaySocket() = default;

public:
	// ��ʼ��
	void Init(
		const char* currentServerPort, // ��ǰ�������Ķ˿�
		const char* remoteServer1IP,  // Զ�̷�����1��IP
		const char* remoteServer1Port, // Զ�̷�����1�Ķ˿�
		const char* remoteServer2IP, // Զ�̷�����2��IP
		const char* remoteServer2Port // Զ�̷�����2�Ķ˿�
	);

	// �ر�
	void Close();

	// �������ݵ������ͻ���
	void SendDataToRemoteServer(const char* data, int length);

	// ע��������յ��ͻ�����Ϣ�ص�����
	void RegisterServerMessageCallbackFunc(ServerMessageCallbackFunc func);

private:
	std::shared_ptr<AsynServerSocket> m_ServerPtr;
	std::vector<AsynClientSocket::ptr> m_ClientPtrVector;
};

#endif // !_TWOWAY_SOCKET_H
