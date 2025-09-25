#include "../../includes/ConfigException.hpp"

ConfigException::ConfigException(string message) : runtime_error(message) {}
