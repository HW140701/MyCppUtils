#include <iostream>
#include "AsynClientSocket.h"

#include "conio.h"
#include <thread>
#include<chrono>

using namespace std;


int main()
{
	AsynClientSocket clientSocket;

	//clientSocket.SetRemoteServerSocketIPAndPort("192.168.0.210", "8661");
	clientSocket.SetRemoteServerSocketIPAndPort("127.0.0.1", "1994");

	if (clientSocket.CreateAndConnectToRemoteServer())
	{
		std::cout << "���ӷ������ɹ�" << std::endl;
	}
	else
	{
		std::cout << "���ӷ�����ʧ��" << std::endl;

		return 0;
	}

	int A = 0;

	while (true)
	{
		std::string str = "HelloWorldHelloWorld"+ std::to_string(A);

		if (clientSocket.SendDataToRemoteServer(str))
		{
			std::cout << "�����������" << str << "�ɹ�" << std::endl;
		}
		else
		{
			std::cout << "�����������" << str << "ʧ��" << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));

		A++;

		if (_kbhit()) // ����а���������
		{
			if (_getch() == 's') //���������q��������ѭ��
			{
				std::string str = "HelloWorldHelloWorld";

				//for (int i = 0; i < 20; ++i)
				//{
				//	if (clientSocket.SendDataToRemoteServer(str))
				//	{
				//		std::cout << "�����������" << str << "�ɹ�" << std::endl;
				//	}
				//	else
				//	{
				//		std::cout << "�����������" << str << "ʧ��" << std::endl;
				//	}
				//}

				//for (int i = 0; i < 100; ++i)
				//{
				//	str += "HelloWorld";
				//}
				//if (clientSocket.SendDataToRemoteServer(str))
				//{
				//	std::cout << "�����������" << str << "�ɹ�" << std::endl;
				//}
				//else
				//{
				//	std::cout << "�����������" << str << "ʧ��" << std::endl;
				//}



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