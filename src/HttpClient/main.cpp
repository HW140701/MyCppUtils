#include <iostream>

#include "HttpClient.h"
#include "AsynHttpClient.h"


using namespace std;

int main()
{
	// ͬ������
	HttpClient client;

	std::string result;

	client.Gets("https://api.ipify.org", result);

	std::cout << "Http Getͬ��������Ϊ��"<<result << std::endl;
	
	// �첽����
	AsynHttpClient asynClient;

	asynClient.RegisterRequestCallbackFunc([](bool isSuccess, std::string response) {
		std::cout <<"Http Get�첽������Ϊ��" <<response << std::endl;
		});

	asynClient.AsynGet("https://api.ipify.org",true);


	getchar();

	return 0;
}