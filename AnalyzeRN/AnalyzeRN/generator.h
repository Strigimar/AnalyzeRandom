#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <array>
#include "QRBG.h"

class Generator
{
public:
	static void gener_rand(unsigned char*, const int);
	static int quantum_rand(unsigned char*, const int);
};