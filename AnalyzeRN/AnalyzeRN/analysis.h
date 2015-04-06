#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <unordered_set>
#include <vector>
#include <map>
#include <algorithm>

#include "statistics.h"

namespace std
{
	template<typename T>
	struct hash<vector<T> >
	{
		typedef vector<T> argument_type;
		typedef size_t result_type;

		result_type operator()(const argument_type& a) const
		{
			hash<T> hasher;
			result_type h = 0;
			for (result_type i = 0; i < a.size(); ++i)
			{
				h = h * 31 + hasher(a[i]);
			}
			return h;
		}
	};
}

class Analysis
{
private:
	int count_byte;

	std::string name;

	unsigned char* observed = nullptr;
	//unsigned char expected[COUNT_BYTE];
	const int max_count_ones = 10;
	const int max_length = 15;
	std::unordered_set< std::vector<int> > masks;
	std::vector<std::vector<int> > best_masks;

	std::vector<int> application_mask();
	
	
	
	std::unordered_set<std::vector<int> > get_combination(int, int);
	void combination_util(const std::vector<int>&, std::vector<int>&, std::unordered_set< std::vector<int> >&, int, int, int, int);
	
	
	std::vector<long> get_position(const std::vector<int>&);
	void gener_variations();


	void write_result(const std::vector<std::map<std::vector<int>, unsigned long* > >&, const std::string&);
	void write_result(const std::vector<int>&, const double);

public:
	Analysis(unsigned char*, const int);
	Analysis() {};
	~Analysis();

	void setCountByte(const int countByte) { this->count_byte = countByte; };
	void setBlock(unsigned char*);

	void gener_mask_variant1();
	void gener_mask_variant2();
	std::vector<int> compute_differences();
	void compute_histogram(std::vector<int>, unsigned long*, unsigned long);//, unsigned long*);
	void test_correct_expected(const unsigned long* expected, int length, std::vector<int> mask);
	static unsigned int get_block(const unsigned char*, int);
	double chi_quadrat_test(unsigned long*, /*unsigned long*,*/ std::vector<int>);


	
	

};

#endif