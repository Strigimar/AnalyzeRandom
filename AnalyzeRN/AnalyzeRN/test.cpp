#include "test.h"

Test::Test(unsigned char* rand, unsigned long limit, int M, int m, int n)
{
	this->limit = limit;
	this->M = M;
	this->m = m;
	this->n = n;
	tested_string = new unsigned char[limit];
	std::memcpy(tested_string, rand, limit);
}
Test::~Test(){
	if (tested_string != nullptr)
	{
		delete tested_string;
		tested_string = nullptr;
	}
		
}

bool Test::test_equal_frequences()
{
	Analysis anal(this->tested_string, limit);
	unsigned char* string_for_matching = new unsigned char[limit * 8];

	convert_to_bits(string_for_matching);

	unsigned long count_ones = OverlappingTemplateMatchings(string_for_matching);

	std::vector<int> mask;
	int count_pos_res = 1 << m;
	unsigned long* frequencies = new unsigned long[count_pos_res];

	std::fill(frequencies, frequencies+count_pos_res, 0);

	for (int i = 0; i < m; i++)
		mask.push_back(i);

	anal.compute_histogram(mask, frequencies, (limit*8 - (mask.back()+1) + 1));
	int c = frequencies[count_pos_res - 1];
	std::cout << c;
	std::cout << " ";
	std::cout << count_ones;
	std::cout << " ";		

	delete[] string_for_matching;
	string_for_matching = nullptr;

	if (frequencies[count_pos_res - 1] == count_ones)
		return true;
	return false;
}
void Test::convert_to_bits(unsigned char result[])
{
	for (int i = 0; i < limit * 8; i++)
	{
		result[i] = Analysis::get_block(this->tested_string, i) & 1;
	}
	
}

unsigned long Test::OverlappingTemplateMatchings(unsigned char str[])
{
	int N = n / M;
	int K = M-m;
	double W_obs; 
	int match;

	unsigned char *sequence_ones;
	if ((sequence_ones = (unsigned char*)calloc(m, sizeof(unsigned char))) == NULL) {
		std::cout << "Memory error" << std::endl;
	}
	else
		for (int i = 0; i<m; i++)
			sequence_ones[i] = 1;


	/*Record the number of occurrences of sequence of 1 in each block by incrementing an array occur[i](where i = 0, .., M/2 - 1),
	such that occur[0] is incremented when there are not occurences of sequence of 1 in block	
	 */
	std::vector<unsigned long> occur(M-m+2);
	std::fill(occur.begin(), occur.end(), 0);
	
	for (int i = 0; i<N; i++) {
		W_obs = 0;
		for (int j = 0; j<M - m + 1; j++) {
			match = 1;
			for (int k = 0; k<m; k++) {
				if (sequence_ones[k] != str[i*M + j + k])
					match = 0;
			}
			if (match == 1)
				W_obs++;
		}
		occur[(int)W_obs]++;
	}
	unsigned long sum = 0;

	for (int i = 1; i < N; i++)
	{
		for (int j = 1; j < m; j++)
		{
			std::string substr(str + (i*M - j), str + (i*M - j) + m);
			
			int k = 0;
			while (k < substr.length() && substr[k] == sequence_ones[k])
			{
				k++;
			}
			if (k == substr.length())
				sum++;

		}
	}

	int length = occur.size();
	for (int i = 0; i < length; i++)
	{
		sum += occur[i] * i;
	}
	
	return sum;
}