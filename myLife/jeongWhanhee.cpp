#include "jeongWhanhee.h"

jeongWhanhee::jeongWhanhee()
{
	age = 1;
	std::cout << "���" << std::endl;
	introduce();
}

void jeongWhanhee::introduce()
{
	std::cout << "���� : " << this->age << std::endl;
}