#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

# include <string>
# include <stdexcept>

using namespace std;

class ConfigException : public runtime_error
{
public:
	ConfigException(const string message);
};

#endif
