#include <string>
#include <string>
#include <sstream>
#include <fstream>

std::string trim_string(std::string const &str)
{
	size_t start = 0;
	size_t end = str.length() - 1;

	for (; str[start] && isspace(str[start]); ++start)
	{
	}

	if (start == str.length())
		return "";

	for (; start < end && isspace(str[end]); --end)
	{
	}

	return str.substr(start, end - start + 1);
}

bool has_space(std::string const &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (isspace(*it))
			return (true);
	}
	return false;
}

bool string_to_word(std::string const &str, std::string &dst)
{
	std::string tmp = trim_string(str);

	if (tmp.length() == 0 || has_space(tmp))
		return true;
	dst = tmp;
	return false;
}

std::string read_file(std::string const &file_path)
{
	std::ifstream ifs(file_path.c_str());
	std::ostringstream ostr;

	if (ifs.fail())
		throw "failed to open file";
	if (ifs.eof())
		return "";
	ostr << ifs.rdbuf();
	if (ostr.fail() || ifs.fail())
		throw "error occured";
	return ostr.str();
}