#pragma once
#include "school.h"

School::School()
{
	this->school_days = 0;
}

School::School(int a)
{
	this -> school_days = a;
}

int School::get_school_days()
{
	return school_days;
}
