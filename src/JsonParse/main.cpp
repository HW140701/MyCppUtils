#include <iostream>
#include "CJsonObject.hpp"

using namespace std;

int main()
{
	neb::CJsonObject jsonObject;

	jsonObject.Add("test", "helloWorld");

	std::cout << "ƴ�յ�jsonΪ" << jsonObject.ToString() << std::endl;

	getchar();

	return 0;
}