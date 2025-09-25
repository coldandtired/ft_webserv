#include "../includes/Utils.hpp"

string Utils::intToString(int num)
{
	ostringstream ss;
	ss << num;
	return ss.str();
}

void Utils::trimString(string &str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	size_t end = str.find_last_not_of(" \t\n\r;");
	if (start != string::npos && end != string::npos)
		str = str.substr(start, end - start + 1);
}

int Utils::stoi(string &str)
{
	int num;

	istringstream(str) >> num;
	return num;
}

int Utils::strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char c1;
	unsigned char c2;

	if (!s1 && !s2)
		return (0);
	while (n--)
	{
		c1 = (unsigned char)*s1++;
		c2 = (unsigned char)*s2++;
		if (c1 != c2)
			return (c1 - c2);
		if (!c1)
			return (0);
	}
	return (0);
}

vector<string> Utils::splitString(const string &str, const char delim)
{
	vector<string> result;
	stringstream ss(str);
	string buffer;

	while (getline(ss, buffer, delim))
		result.push_back(buffer);

	return result;
}
