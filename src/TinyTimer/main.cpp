#include <iostream>
#include "TinyTimer.h"

using namespace std;

int main()
{
	TinyTimer timer;
	timer.AsyncOnceExecute(1000, []() {
		std::cout << "�ӳ�һ���ӡһ��" << std::endl; 
		});

	getchar();

	return 0;
}