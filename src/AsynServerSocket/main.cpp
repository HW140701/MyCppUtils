#include <iostream>
#include "AsynServerSocket.h"

#include "conio.h"

using namespace std;

int main()
{
	AsynServerSocket serverSocket;
	serverSocket.SetServerPort("1994");

	serverSocket.CreateServerSocket();

	std::cout << "�������������" << std::endl;

	while (true)
	{
		if (_kbhit()) // ����а���������
		{
			if (_getch() == 'e')
			{
				std::cout << "�رշ�����" << std::endl;

				serverSocket.CloseServer();

				break;
			}

		}
	}

	getchar();

	return 0;
}


//#include <iostream>
//#include<thread>
//#include<ws2tcpip.h>
//#include<Windows.h>
//#include<string>
//#include<string.h>
//
//using namespace std;
//
//class tcpthread
//{
//public:
//	void Main()
//	{
//		char buf[1024] = { 0 };//������Ϣ����󳤶ȣ���λbuf
//		while (true)
//		{
//			int recvlen = recv(client, buf, sizeof(buf) - 1, 0);//windowsû��read������linux����
//			if (recvlen <= 0)break;//û���յ�
//			if (strstr(buf, "q") != NULL)//��q�˳�
//			{
//				char re[] = "quit success!!!\n";
//				send(client, re, strlen(re) + 1, 0);//��1����Ϊ����\0
//				break;
//			}
//			int sendlen = send(client, "ok\n", 3, 0);//linux����ʹ��write
//			cout << "receive:" << buf << endl;
//			//len�ǽ������ݵ�ʵ�ʴ�С��len<=buf���ȣ�������1024��
//		}
//
//
//		closesocket(client);//�ر�����
//
//	}
//	int client = 0;
//};
//
//int main(int argc, char* argv[])
//{
//	//��ʼ����̬���ӿ�
//	WSADATA ws;
//	WSAStartup(MAKEWORD(2, 2), &ws);//22�ǰ汾��,���ض�̬���ӿ�
//	int sock = socket(AF_INET, SOCK_STREAM, 0);//AF_INETָ������TCP/IPЭ��,SOCK_STREAM��TCP��Э�飨�����UDP������
//	cout << sock << endl;//��ӡ���id��ʧ�ܷ��ظ�ֵ
//	//ʧ����ʾ
//	if (sock == -1)
//	{
//		cout << "create socket failed!" << endl;
//		return -1;
//	}
//
//	//���Զ˿ں�
//	unsigned short port = 1994;
//	if (argc > 1)
//	{
//		port = atoi(argv[1]);
//	}
//
//	//����TCP��صĽṹ��
//	sockaddr_in saddr;
//	saddr.sin_family = AF_INET;//ʹ��TCP
//	saddr.sin_port = htons(port);//�����ֽ���ת�����ֽ���
//	//X86�ܹ���С�˵Ķ������ֽ����Ǵ�˵ģ�
//	//Linux��һ����С��linuxʹ�õ�Ҳ�Ǻ���·�ֽ���һ���Ļ�ת��Ҳֻ��һ���յĺ꣬
//	//��ʱ�����п��ޣ������Ǽ�����Ҫ�������
//	saddr.sin_addr.s_addr = htonl(0);//�������ָ��������0���������˼
//
//	if (bind(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0)//�󶨶˿ںŵ����洴����socket,���ж��Ƿ�ɹ�
//	{
//		cout << "bind port " << port << " failed!" << endl;
//		return -2;
//	}
//	else
//	{
//		cout << "bind port " << port << " success!" << endl;
//	}
//
//	listen(sock, 10);//�������������ӣ�10���б��С���׽��ֽ��ն��е�����С 
//	//acceptÿ����һ�ζ��оͻ����һ��
//
//	while (true)
//	{
//		sockaddr_in caddr;
//		socklen_t len = sizeof(caddr);
//		int client = accept(sock, (sockaddr*)&caddr, &len);//ȡ��Ϣ
//		if (client <= 0)break;
//
//		//cout << client << endl;
//		//char* ip = inet_ntoa(caddr.sin_addr);
//		//unsigned short cport = ntohs(caddr.sin_port);//�����ֽ���ת�����ֽ���
//		//cout << "client ip is " << ip << " port is " << cport << endl;
//		tcpthread* th = new tcpthread();
//		th->client = client;
//		thread sth(&tcpthread::Main, th);
//		sth.detach();//�ͷ����߳�ӵ�е����̵߳���Դ
//	}
//	closesocket(sock);
//
//	return 0;
//}