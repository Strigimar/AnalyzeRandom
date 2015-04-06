#include "file.h"

bool File::exist_file()
{
	if (FILE *file = fopen(name_file.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

void File::clear_file()
{
	if (exist_file())
	{
		std::ofstream file(name_file, std::ios::out | std::ios::trunc);
		file.close();
	}
	else
	{
		std::ofstream outfile(name_file);
		outfile.close();
	}
}

bool File::read_file(unsigned char* str, const int length)
{
	if (exist_file())
	{
		const int l = length;
		std::ifstream file(name_file, std::ifstream::binary);
		char* pom = new char[length];
		file.read(pom, length);
		std::memcpy(str, pom, length);
		
		delete[] pom;
		file.close();
		return true;
	}
	return false;
}
bool File::write_file(const std::string& text, std::ios::openmode open)
{
	if (exist_file())
	{
		if (open != std::ios::app && open != std::ios::trunc)
			return false;
		std::ofstream file(name_file, std::ios::out | open);
		file << text.c_str();
		file.close();
		return true;
	}
	return false;
}
bool File::write_file(unsigned char* text, const int length)
{
	if (exist_file())
	{
		std::ofstream file(name_file, std::ofstream::binary);
		file.write((const char*)text, length);
		file.close();
		return true;
	}
	return false;
}
bool File::is_empty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}