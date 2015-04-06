#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>


class File
{
private:
	std::string name_file;

	bool exist_file();
public:

	File(std::string name) : name_file(name) {}
	bool read_file(unsigned char*, const int);
	bool write_file(const std::string&, std::ios::iostate);
	bool write_file(unsigned char*, const int);
	void clear_file();
	bool is_empty(std::ifstream&);

};