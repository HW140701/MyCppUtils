#include <iostream>

using namespace std;

#include "PortOccupation.h"

int main()
{
	if (PortOccupation::PortIsOccupied("866"))
	{
		std::cout << "�˿ڱ�ռ��" << std::endl;
		//PortOccupation::KillAllOccupyPortProcess("1994");
	}
	else
	{
		std::cout << "�˿�δ��ռ��" << std::endl;
	}
	
	getchar();

	return 0;
}