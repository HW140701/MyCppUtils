#include "AsynClientSocket.h"

#include <sstream>
#include <iostream>
#include <thread>

std::string ASCII2UTF8(const char* cont)
{
	if (NULL == cont)
	{
		return std::string("");
	}

	int num = MultiByteToWideChar(CP_ACP, NULL, cont, -1, NULL, NULL);
	wchar_t* buffw = new wchar_t[(unsigned int)num];
	MultiByteToWideChar(CP_ACP, NULL, cont, -1, buffw, num);

	int len = WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	char* lpsz = new char[(unsigned int)len + 1];
	WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	delete[] buffw;

	std::string rtn(lpsz);
	delete[] lpsz;
	return rtn;
}


void intToByte(int i, BYTE abyte[], int size = 4)
{
	memset(abyte, 0, sizeof(byte) * size);

	abyte[3] = (byte)(0xff & i);

	abyte[2] = (byte)((0xff00 & i) >> 8);

	abyte[1] = (byte)((0xff0000 & i) >> 16);

	abyte[0] = (byte)((0xff000000 & i) >> 24);

}


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

	// ���÷�����ģʽ����
	int iMode = 1;
	result = ioctlsocket(m_Socket, FIONBIO, (unsigned long*)&iMode);
	if (SOCKET_ERROR == result)
	{
		WSACleanup();
		return false;
	}

	// ���ó�ʱʱ��
	struct timeval tm;
	tm.tv_sec = 2;
	tm.tv_usec = 0;

	// �������ӿͻ���
	result = connect(m_Socket, (LPSOCKADDR)&m_RemoteServerAddress, sizeof(m_RemoteServerAddress));
	if (-1 != result)
	{
		return true;
	}
	else
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(m_Socket, &set);

		if (select(-1, NULL, &set, NULL, &tm) <= 0)
		{
			return false;
		}
		else
		{
			int error = -1;
			int optLen = sizeof(int);
			getsockopt(m_Socket, SOL_SOCKET, SO_ERROR, (char*)&error, &optLen);

			// ֮��������ĳ���д����Ŀ���������ʽ�� ��Ϊ�˸�ֱ�ۣ� ����ע��
			if (0 != error)
			{
				//�ر��׽���
				closesocket(m_Socket);
				//�ͷ��׽�����Դ
				WSACleanup();

				return false;
			}
			else
			{
				// ���Ϊ����socket
				iMode = 0;
				ioctlsocket(m_Socket, FIONBIO, (u_long FAR*) & iMode); //����Ϊ����ģʽ

				return true;
			}
		}
	}



	// ���ӷ�����
	//while (true)
	//{
	//	result = connect(m_Socket, (LPSOCKADDR)&m_RemoteServerAddress, sizeof(m_RemoteServerAddress));
	//	if (SOCKET_ERROR == result)
	//	{
	//		int nErrCode = WSAGetLastError();
	//		if (WSAEWOULDBLOCK == nErrCode || WSAEINVAL == nErrCode)    //���ӻ�û�����
	//		{
	//			continue;
	//		}
	//		else if (WSAEISCONN == nErrCode)//�����Ѿ����
	//		{
	//			break;
	//		}
	//		else//����ԭ������ʧ��
	//		{
	//			//�ر��׽���
	//			closesocket(m_Socket);
	//			//�ͷ��׽�����Դ
	//			WSACleanup();

	//			std::cout << "Զ�̷���������ʧ��" << std::endl;

	//			return false;
	//		}
	//	}
	//	else if(result == 0)
	//	{
	//		break;
	//	}
	//}

	//result = connect(m_Socket, (LPSOCKADDR)&m_RemoteServerAddress, sizeof(m_RemoteServerAddress));
	//if (SOCKET_ERROR == result)
	//{
	//	std::cout << "���ӷ�����ʧ��" << std::endl;
	//	return false;
	//}


	//std::cout << "����Զ�̷������ɹ�" << std::endl;

	//return true;
}

bool AsynClientSocket::SendDataToRemoteServer(const std::string& str)
{
	std::shared_ptr<std::thread> threadPtr = std::make_shared<std::thread>([=] {
		std::cout << "�첽���߳�ִ��" << std::endl;	

		std::string utf8Str = str;

		int length = str.length();

		char* messageByte = new char[utf8Str.length() + 4];

		byte dataLengthByteArray[4];

		intToByte(utf8Str.length(), dataLengthByteArray);

		memcpy(messageByte, dataLengthByteArray, 4);

		memcpy(messageByte + 4, utf8Str.c_str(), utf8Str.length());

		int result = send(m_Socket, (char*)messageByte, sizeof(char) * utf8Str.length() + 4, 0);

		if (SOCKET_ERROR == result)
		{
			//�ر��׽���
			closesocket(m_Socket);
			//�ͷ��׽�����Դ
			WSACleanup();

			return false;
		}

		delete[] messageByte;

		std::cout << "�첽���߳����" << std::endl;

		});
	threadPtr->detach();


	//std::string utf8Str = ASCII2UTF8(str.c_str());

	//std::string utf8Str = str;

	//int length = str.length();

	//char* messageByte = new char[utf8Str.length() + 4];

	//byte dataLengthByteArray[4];

	//intToByte(utf8Str.length(), dataLengthByteArray);

	//memcpy(messageByte, dataLengthByteArray, 4);

	//memcpy(messageByte + 4, utf8Str.c_str(), utf8Str.length());

	//int result = send(m_Socket, (char*)messageByte, sizeof(char) * utf8Str.length() + 4, 0);

	//if (SOCKET_ERROR == result)
	//{
	//	//�ر��׽���
	//	closesocket(m_Socket);
	//	//�ͷ��׽�����Դ
	//	WSACleanup();

	//	return false;
	//}

	//delete[] messageByte;

	return true;

}

void AsynClientSocket::CloseClient()
{
	//�ر��׽���
	closesocket(m_Socket);
	//�ͷ��׽�����Դ
	WSACleanup();
}
