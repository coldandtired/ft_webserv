#ifndef CGI_HPP
#define CGI_HPP

# include <fcntl.h>
# include <iostream>
# include <map>
# include <string>
# include <string.h>
# include <sys/wait.h>

# include "Request.hpp"
# include "webserv.hpp"

using namespace std;

class Cgi
{
	private:
		map<string, string> envVars;
		Request request;
		void setVars();
		void executeBin(int &file, char **argv, char **envp);

	public:
		int	pipeIn[2];
		int	pipeOut[2];
		int pid;
		void run();
		void writeData();
		void readData();

		void setRequest(Request);
};

#endif
