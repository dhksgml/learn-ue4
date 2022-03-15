#include "jeongWhanhee.h"

jeongWhanhee::jeongWhanhee()
{
	age = 1;
	std::cout << "출생" << std::endl;
	introduce();
}

void jeongWhanhee::introduce()
{
	std::cout << "나이 : " << this->age << std::endl;
}