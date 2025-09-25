#include "../../includes/ServerConnectionException.hpp"

ServerConnectionException::ServerConnectionException(string message) : runtime_error(message) {}
