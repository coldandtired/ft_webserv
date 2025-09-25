#ifndef UTILS_HPP
#define UTILS_HPP

# include <istream>
# include <ostream>
# include <sstream>
# include <string>
# include <vector>

using namespace std;

class Utils
{
	public:
		static string intToString(int num);
		static void trimString(string &str);
		static int stoi(string &str);
		static int strncmp(const char *s1, const char *s2, size_t n);
		static vector<string> splitString(const string &str, const char delim);
};

#endif
