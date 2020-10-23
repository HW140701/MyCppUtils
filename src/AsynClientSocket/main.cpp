#include <iostream>
#include "AsynClientSocket.h"

#include "conio.h"

using namespace std;

int main()
{
	AsynClientSocket clientSocket;

	clientSocket.SetRemoteServerSocketIPAndPort("127.0.0.1", "1994");

	clientSocket.CreateAndConnectToRemoteServer();

	while (true)
	{
		if (_kbhit()) // ����а���������
		{
			if (_getch() == 's') //���������q��������ѭ��
			{
				std::string str = "���";

				if (clientSocket.SendDataToRemoteServer(str.c_str(), str.length()))
				{
					std::cout << "�����������" << str << "�ɹ�" << std::endl;
				}
				else
				{
					std::cout << "�����������" << str << "ʧ��" << std::endl;
				}			
			}
			else if (_getch() == 'e')
			{
				clientSocket.CloseSocket();

				break;
			}

		}
	}


	getchar();

	return 0;
}