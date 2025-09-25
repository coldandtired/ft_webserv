#ifndef SERVERCONNECTIONEXCEPTION_HPP
#define SERVERCONNECTIONEXCEPTION_HPP

# include <string>
# include <stdexcept>

using namespace std;

class ServerConnectionException : public runtime_error
{
public:
	ServerConnectionException(const string message);
};

#endif
