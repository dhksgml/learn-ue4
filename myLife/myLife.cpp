#pragma once
#include "myLife.h"

myLife::myLife()
{
	std::cout << "인생 시작" << std::endl << std::endl;
	jeongWhanhee jwh;
	School school;
}

int main()
{

	/*School* s = new School(ELEMENTARY_SCHOOL);
	if (s->get_school_days() == 0)
		std::cout << "초등학교 : ";
	std::cout << s->get_school_days() << std::endl;*/

	//jeongWhanhee* j = new jeongWhanhee();

	myLife* life = new myLife();
}