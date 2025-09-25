#include "../../includes/InvalidRequestException.hpp"

InvalidRequestException::InvalidRequestException(string message) : runtime_error(message) {}
