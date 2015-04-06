#ifndef TEST_H
#define TEST_H

#include <algorithm> 
#include <iostream>
#include "analysis.h"

class Test{

private:
	unsigned char* tested_string = nullptr;
	unsigned long limit;
	int M, m, n;

	unsigned long OverlappingTemplateMatchings(unsigned char []);
	void convert_to_bits(unsigned char[]);

public:
	Test(unsigned char*, unsigned long, int, int, int);
	bool test_equal_frequences();
	~Test();
};

#endif