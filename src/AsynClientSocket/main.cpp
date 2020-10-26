#include <iostream>
#include "AsynClientSocket.h"

#include "conio.h"

using namespace std;

int main()
{
	AsynClientSocket clientSocket;

	clientSocket.SetRemoteServerSocketIPAndPort("192.168.0.217", "1994");

	clientSocket.CreateAndConnectToRemoteServer();

	while (true)
	{
		if (_kbhit()) // ����а���������
		{
			if (_getch() == 's') //���������q��������ѭ��
			{
				std::string str = "HelloWorld";

				if (clientSocket.SendDataToRemoteServer(str))
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
				clientSocket.CloseClient();

				break;
			}

		}
	}


	getchar();

	return 0;
}