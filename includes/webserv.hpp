#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <map>
# include <string>

# define BUFFER_SIZE 400000
# define PYTHONBIN "/usr/bin/python3"
# define PHPBIN "/usr/bin/php-cgi"
# define PERLBIN "/usr/bin/perl"

using namespace std;

typedef struct s_resources
{
	map<string, string> mimeTypes;
	map<int, string> statusCodes;
} t_resources;

#endif
