#include "AsynClientSocket.h"

#include <sstream>
#include <iostream>

void AsynClientSocket::SetRemoteServerSocketIPAndPort(const char* ip, const char* port)
{
	// ����Զ�̷�������ַ
	SOCKADDR_IN		remoteServerAddress;

	remoteServerAddress.sin_family = AF_INET; // ��Ҫ���ӵ�IPV4Э��
	inet_pton(AF_INET, ip, (void*)&remoteServerAddress.sin_addr.S_un.S_addr);	// ��Ҫ���ӵ�Զ�̷�������IP��ַ

	// �˿��ַ���תint��
	std::istringstream intStr(port);	// ��Ҫ���ӵ�Զ�̷������Ķ˿�
	int tempPort;
	intStr >> tempPort;

	remoteServerAddress.sin_port = htons(tempPort);

	m_RemoteServerAddress = remoteServerAddress;
}

bool AsynClientSocket::CreateAndConnectToRemoteServer()
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
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_Socket)
	{
		WSACleanup();
		return false;
	}

	// �����׽��ַ�����ģʽ
	//unsigned long ul = 1;
	//result = ioctlsocket(m_Socket, FIONBIO, (unsigned long*)&ul);
	//if (SOCKET_ERROR == result)
	//{
	//	WSACleanup();
	//	return false;
	//}

	// ���ӷ�����
	while (true)
	{
		result = connect(m_Socket, (LPSOCKADDR)&m_RemoteServerAddress, sizeof(m_RemoteServerAddress));
		if (SOCKET_ERROR == result)
		{
			int nErrCode = WSAGetLastError();
			if (WSAEWOULDBLOCK == nErrCode || WSAEINVAL == nErrCode)    //���ӻ�û�����
			{
				continue;
			}
			else if (WSAEISCONN == nErrCode)//�����Ѿ����
			{
				break;
			}
			else//����ԭ������ʧ��
			{
				//�ر��׽���
				closesocket(m_Socket);
				//�ͷ��׽�����Դ
				WSACleanup();

				std::cout << "Զ�̷���������ʧ��" << std::endl;

				return false;
			}
		}
		else if(result == 0)
		{
			break;
		}
	}

	std::cout << "����Զ�̷������ɹ�" << std::endl;

	return true;
}

bool AsynClientSocket::SendDataToRemoteServer(const char* data, int length)
{
	char* tempData = new char[length];

	memset(tempData, 0, sizeof(char) * length);

	memcpy(tempData, data, length);

	int result = send(m_Socket, tempData, sizeof(char) * length, 0);

	if (SOCKET_ERROR == result)
	{
		//�ر��׽���
		closesocket(m_Socket);
		//�ͷ��׽�����Դ
		WSACleanup();

		return false;
	}

	return true;

	delete[] tempData;
}

void AsynClientSocket::CloseClient()
{
	//�ر��׽���
	closesocket(m_Socket);
	//�ͷ��׽�����Դ
	WSACleanup();
}
