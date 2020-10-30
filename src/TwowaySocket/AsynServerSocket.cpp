#include "AsynServerSocket.h"

#include <sstream>

#include <iostream>


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

std::string UTF82ASCII(const char* cont)
{
	if (NULL == cont)
	{
		return std::string("");
	}
	int num = MultiByteToWideChar(CP_UTF8, NULL, cont, -1, NULL, NULL);
	wchar_t* buffw = new wchar_t[(unsigned int)num];
	MultiByteToWideChar(CP_UTF8, NULL, cont, -1, buffw, num);
	int len = WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	char* lpsz = new char[(unsigned int)len + 1];
	WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	delete[] buffw;
	std::string rtn(lpsz);
	delete[] lpsz;
	return rtn;
}

//4�ֽ� BYTE[] ת int 
int bytesToInt(BYTE bytes[])
{
	int addr = bytes[3] & 0xFF;

	addr |= ((bytes[2] << 8) & 0xFF00);

	addr |= ((bytes[1] << 16) & 0xFF0000);

	addr |= ((bytes[0] << 24) & 0xFF000000);

	//long int result = (x[0] << 24) + (x[1] << 16) + (x[2] << 8) + x[3];   

	return addr;
}


AsynServerSocket::AsynServerSocket()
{
	m_IsCloseServer = false;
}

void AsynServerSocket::SetServerPort(const char* port)
{
	// ���õ�ǰ��������ַ
	SOCKADDR_IN		serverAddress;

	serverAddress.sin_family = AF_INET;

	inet_pton(AF_INET, "0.0.0.0", (void*)&serverAddress.sin_addr.S_un.S_addr);

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
	//std::cout << "��ʼ��Windows�׽��ֳɹ�" << std::endl;

	// �����׽���
	m_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_ServerSocket)
	{
		WSACleanup();
		return false;
	}
	//std::cout << "�����������׽��ֳɹ�" << std::endl;

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

	//std::cout << "�󶨷������׽��ֳɹ�" << std::endl;

	// ��ʼ����
	result = listen(m_ServerSocket, 5);
	if (SOCKET_ERROR == result)
	{
		closesocket(m_ServerSocket);
		WSACleanup();
		return false;
	}

	//std::cout << "�������׽��ֿ�ʼ����" << std::endl;

	// ��ʼ���ܿͻ�������
	m_ServerMainThreadPtr = std::make_shared<std::thread>(&AsynServerSocket::ServerProcessThreadFunction,this);

	return true;
}

void AsynServerSocket::CloseServer()
{
	// �رշ��������÷������߳��˳�
	m_IsCloseServer = true;

	// �ر��׽��ֺ�������Դ
	closesocket(m_ServerSocket);
	WSACleanup();

	// �ȴ������������߳��˳�
	m_ServerMainThreadPtr->join();

	// �ȴ����н����������߳��˳�

	std::map<int, std::shared_ptr<std::thread>>::iterator iter1;
	for (iter1 = m_SingleSocketRecvThreadMap.begin(); iter1 != m_SingleSocketRecvThreadMap.end(); iter1++)
	{
		closesocket(iter1->first);
	}

	std::map<int, std::shared_ptr<std::thread>>::iterator iter;
	for (iter = m_SingleSocketRecvThreadMap.begin(); iter != m_SingleSocketRecvThreadMap.end(); iter++)
	{
		if (iter->second != nullptr)
		{
			iter->second->join();
		}
	}

	// ɾ��map�е��������߳�
	std::map<int, std::shared_ptr<std::thread>>::iterator iter2 = m_SingleSocketRecvThreadMap.begin();

	while (iter2 != m_SingleSocketRecvThreadMap.end())
	{
		iter2->second.reset();

		iter2 = m_SingleSocketRecvThreadMap.erase(iter2);
	}
	m_SingleSocketRecvThreadMap.clear();

}

std::string AsynServerSocket::GetMessage()
{
	std::string result;

	return "";
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

		if (clientSocket != SOCKET_ERROR)
		{
			std::cout << "һ���ͻ��������ӵ���������socket�ǣ�" << clientSocket << std::endl;

			//int threadIndex = m_SingleSocketRecvThreadMap.size();


			std::shared_ptr<std::thread> singleSocketRecvThread = std::make_shared<std::thread>(&AsynServerSocket::SingleSocketReciveThreadFunction, this, clientSocket, clientSocket);

			m_SingleSocketRecvThreadMap[clientSocket] = singleSocketRecvThread;
		}
	}

	std::cout << "�������������߳���ȷ�˳�" << std::endl;
}

void AsynServerSocket::SingleSocketReciveThreadFunction(int socket, int threadIndex)
{
	int clientSocket = socket;
	int clientThreadIndex = threadIndex;

	while (true)
	{
		if (m_IsCloseServer)
		{
			closesocket(socket);

			break;
		}


		char recvBuf[1024] = { 0 };

		int recvlen = recv(clientSocket, recvBuf, sizeof(recvBuf), 0);

		// �ͻ����ѹر�����
		if (recvlen == 0)
		{
			std::cout << "socket���ӹر�" << std::endl;

			closesocket(socket);

			break;
		}
		// ����
		else if (recvlen == -1)
		{

			break;
		}
		else if (recvlen > 0)
		{
			//recvBuf[recvlen] = '\0';

			// �õ�Э���ʽ�����ݵĳ���
			char dataLengthStr[4];
			memcpy(dataLengthStr, recvBuf, 4);
			int dataLength = bytesToInt((BYTE*)dataLengthStr);

			// �õ���ǰ��Ϣ������ݵĳ���
			int messageLength = recvlen - 4;

			char* dataStr = new char[dataLength + 1];

			memset(dataStr, 0, dataLength);

			memcpy(dataStr, recvBuf + 4, messageLength);

			int writeDataLength = messageLength;

			while (writeDataLength < dataLength)
			{
				char buffer[1024] = { 0 };

				int recvbufferLen = recv(clientSocket, buffer, sizeof(buffer), 0);

				// �ͻ����ѹر�����
				if (recvbufferLen == 0)
				{
					//std::cout << "socket���ӹر�" << std::endl;

					closesocket(socket);

					break;
				}
				// ����
				else if (recvbufferLen == -1)
				{


					break;
				}
				else if (recvbufferLen > 0)
				{
					int tempWriteDataLength = writeDataLength + recvbufferLen;

					if (tempWriteDataLength >= dataLength)
					{
						memcpy(dataStr + writeDataLength , buffer, dataLength - writeDataLength);

						//break;
					}
					else
					{
						memcpy(dataStr + writeDataLength , buffer, recvbufferLen);
					}

					writeDataLength = tempWriteDataLength;

				}
			}

			dataStr[dataLength] = '\0';

			// �����ǰ������û�йرղ��������ѹ�����ݣ���ֹ���߳��ڴ���Ĺ��̣����������ر���Ȼѹ�����ݵ��µ�bug
			if (!m_IsCloseServer)
			{
				std::cout << "�յ��ͻ��˵����ݣ�" << UTF82ASCII(dataStr) << std::endl;
			}	

			delete[] dataStr;

		}
	}

	std::cout << "���������������������߳���ȷ�˳�" << std::endl;
}
