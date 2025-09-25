#include "../../includes/ServerRunningException.hpp"

ServerRunningException::ServerRunningException(string message) : runtime_error(message) {}
