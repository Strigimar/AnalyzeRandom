#include "analysis.h"
#include "file.h"
#include <set>

#include "generator.h"

Analysis::Analysis(unsigned char* obs, const int count_byte)
{
	this->count_byte = count_byte;
	this->observed = new unsigned char[count_byte];
	std::memcpy(observed, obs, this->count_byte);
}
void Analysis::setBlock(unsigned char* string)
{
	if (this->observed != nullptr)
	{
		delete[] this->observed;
	}
	this->observed = new unsigned char[count_byte];
	std::memcpy(observed, string, this->count_byte);
}
Analysis::~Analysis()
{
	if (this->observed != nullptr)
	{
		delete[] this->observed;
		this->observed = nullptr;
	}
}

std::vector<int> Analysis::compute_differences()
{
	return application_mask();
	
	/*if (best_masks.size() > 1)
	{
		gener_variations();
		application_mask();
	}*/

}
void Analysis::gener_variations()
{
	int size = best_masks.size();
	masks.clear();
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
		if (i != j)
		{
			std::vector<int> union_mask;
			union_mask.reserve(best_masks[i].size() + best_masks[j].size());
			union_mask.insert(union_mask.end(), best_masks[i].begin(), best_masks[i].end());
			
			std::vector<int> shifted_mask(best_masks[j].begin(), best_masks[j].end());
			int size_mask = best_masks[i][best_masks[i].size()-1] + 1;
			std::for_each(shifted_mask.begin(), shifted_mask.end(), [&](int& elem){ elem += size_mask; });

			union_mask.insert(union_mask.end(), shifted_mask.begin(), shifted_mask.end());
			
			masks.insert(union_mask);
		}
		}
}

std::vector<int> Analysis::application_mask()
{
	double chi_quadr;
	std::vector<int> best_mask; 
	double best_pi = 1;

	for (std::vector<int> mask : this->masks)
	{
		int len_mask = mask.back() + 1;
		
		unsigned long* freq_o = new unsigned long[(1 << len_mask)];
		std::fill(freq_o, freq_o + (1 << len_mask), 0);

		compute_histogram(mask, freq_o, this->count_byte * 8 - len_mask + 1);

		chi_quadr = chi_quadrat_test(freq_o, mask);
		
		Statistics *st = new Statistics();
		double pi = st->get_chi2Probability(((1 << mask.size()) - 1), chi_quadr);
		/*std::cout << pi;
		std::cout << "  ";*/
		if (pi < best_pi && pi != 0)
		{
			best_mask = mask;
			best_pi = pi;
		}

		/*if (pi <= 0.05)
		{
			write_result(mask, chi_quadr);
			best_masks.push_back(mask);
		}*/
		
		delete[] freq_o;
		delete st;
		freq_o = nullptr;

	}
	return best_mask;

}

void Analysis::compute_histogram(std::vector<int> pos, unsigned long* obs, unsigned long limit)//, unsigned long* exp)
{
	unsigned int mask = 0;
	for (int pos_one : pos)
	{
		mask += (1 << pos_one);
	}

	for (int i = 0; i < limit ; i++)
	{
		obs[get_block(observed, i) & mask]++;
	
		//exp[(get_block(expected, i) & mask)]++;
	}
}

void Analysis::gener_mask_variant1()
{
	int count = (1 << max_count_ones);
	for (int i = 3; i < count; i = i + 2)
	{
		std::vector<int> ind;
		int limit = ceil(log2(i));

		for (int j = 0; j < limit; j++)
		{
			if ((((1 << j) & i)>>j) == 1)
				ind.push_back(j);
		}
		masks.insert(ind);
	}
}
void Analysis::gener_mask_variant2()
{

	for (int i = 2; i < max_count_ones; i++)
	{
		std::vector<bool> v(max_length);
		std::fill(v.begin() + i, v.end(), true);

		do {
			int *mask = new int[i];
			int j = 0, firstValue = -1;

			for (int k = 0 ; k < max_length; ++k) {
				if (!v[k]) {
					if (firstValue == -1)
						firstValue = k;
					mask[j] = k - firstValue;
					j++;
				}
			}
			std::vector<int> conv(mask, mask + i);

			if (masks.find(conv) == masks.end())
				masks.insert(std::vector<int>(conv));
			delete[] mask;

		} while (std::next_permutation(v.begin(), v.end()));
	}
}

unsigned int Analysis::get_block(const unsigned char* array, int offset)
{
	return (*(unsigned int*)(array + (offset >> 3))) >> (offset & 7);
}

double Analysis::chi_quadrat_test(unsigned long* freq_o /*, unsigned long* freq_e*/, std::vector<int> mask)
{
	//For whole sequence of bits
	long countPair = this->count_byte * 8 - (mask.back() + 1) + 1; //(mask.back() + 1) == length of mask
	long double expected = countPair / (1 << mask.size());
	std::vector<long> positions = get_position(mask);

	long double chi_quadrat = 0;

	std::for_each(positions.begin(), positions.end(), [&](long i){
		if (freq_o[i] > 5)
		{
			chi_quadrat += (pow(((double)freq_o[i] - expected), 2.0) / expected);
		}
	});

	return chi_quadrat;
}
void Analysis::write_result(const std::vector<int>& mask, const double chi_quadr)
{
	std::stringstream out;
	
	Statistics* st = new Statistics();

	unsigned int m = 0;
	for (int pos_one : mask)
	{
		m += pow(2, pos_one);
	}
	for (int j = floor(log2(m)); j >= 0; j--)
	{
		out << ((m & (1 << j)) >> j);
	}
	out << " || X^2 = ";
	out << std::fixed << chi_quadr;
	out << " || p-value = ";
	out << st->get_chi2Probability((1 << mask.size()) - 1, chi_quadr);
	out << "\n";

	std::string text = out.str();

	File* f = new File(name);
	f->write_file(text, std::ios::app);

	delete st;
	delete f;
	st = nullptr;
	f = nullptr;
}

/*vyber pozic, ktere maji smysl pri maskovani, napr. pri masce 0, 2 nema index 010 v expected smysl, bude vzdy nulova*/
std::vector<long> Analysis::get_position(const std::vector<int>& mask)
{
	std::vector<long> positions;
	int limit = (1 << mask.size());
	int mask_size = mask.size();
	for (int i = 0; i < limit; i++)
	{
		long k = 0;
		for (short j = 0; j < mask_size; j++)
		{
			k += ((i & (1<<j)) >> j)*(1<<mask[j]);
		}
		positions.push_back(k);
	}
	return positions;
}
