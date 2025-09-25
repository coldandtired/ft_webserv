#ifndef SERVERRUNNINGEXCEPTION_HPP
#define SERVERRUNNINGEXCEPTION_HPP

# include <string>
# include <stdexcept>

using namespace std;

class ServerRunningException : public runtime_error
{
public:
	ServerRunningException(const string message);
};

#endif
