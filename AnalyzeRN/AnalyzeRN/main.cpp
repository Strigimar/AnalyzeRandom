#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <chrono>
#include <map>
#include <unordered_set>
#include <set>
#include <array>
#include <vector>
#include <iterator>
#include "QRBG.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "generator.h"
#include "analysis.h"
#include "test.h"
#include "file.h"
#include "KolmogorovSmirnovDist.h"
#include "KolmogorovSmirnovDist.c"

#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC


void writeFile(std::vector<int> mask, double p_value)
{
	File *f = new File("resultKolgomor.txt");
	std::stringstream out;
	for (int i = 0; i < mask.size(); i++)
	{
		out << mask[i];
		out << " ";
	}
	out << "p_value: ";
	out << p_value;
	out << "\n";
	std::string text = out.str();
	f->write_file(text, std::ios::app);
	delete f;
}
int main()
{

	const int count_byte = 125000000;
	const int length_block = count_byte/1000;
	
	unsigned char *observed= new unsigned char[count_byte];
	Generator::gener_rand(observed, count_byte);

	std::vector<int> best_mask;
	std::vector<double> pi_values;
	Analysis *anal = new Analysis();
	anal->setCountByte(length_block);
	anal->gener_mask_variant1();

	for (int i = 0; i < 1000; i++)
	{
		unsigned char *block = new unsigned char[length_block];
		std::memcpy(block, observed + (i * length_block), length_block);

		anal->setBlock(block);
		if (i>0)
		{
			int len_mask = best_mask.back() + 1;
			unsigned long* obs = new unsigned long[(1 << len_mask)];
			std::fill(obs, obs + (1 << len_mask), 0);

			anal->compute_histogram(best_mask, obs, length_block * 8 - len_mask + 1);
			double chi_quadr = anal->chi_quadrat_test(obs, best_mask);
			Statistics *st = new Statistics();
			double p_value = st->get_chi2Probability(1 << (best_mask.size() - 1), chi_quadr);
			pi_values.push_back(p_value);
			writeFile(best_mask, p_value);
			delete[] obs;
			delete st;
		}
		best_mask = anal->compute_differences();
		
		delete[] block;
	}

	File *f = new File("resultKolgomor.txt");
	Statistics *stat = new Statistics();
	bool result = stat->kolmogor_smirn_test(pi_values);

	delete anal;
	/*srand(time(NULL));
	for (int i = 0; i < 10000; i++)
	{
		
		Generator::gener_rand(observed, count_byte);


		Test* t = new Test(observed, count_byte, 10000, 20, count_byte * 8);
		std::cout << (int)t->test_equal_frequences();
		std::cout << std::endl;

		delete t;
	}*/
	
	delete[] observed;

	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
 	return 0;
}
