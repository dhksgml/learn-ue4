#pragma once
#include "jeongWhanhee.h"

enum School_days
{
	ELEMENTARY_SCHOOL,
	MIDDLE_SCHOOL,
	HIGH_SCHOOL,
	UNIVERSITY
};

class School
{
private:
	int school_days;
	char school_activity[100];

public:
	School();
	School(int a);
	int get_school_days();
};

