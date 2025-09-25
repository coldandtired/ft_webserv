#include "../includes/Cgi.hpp"

void Cgi::setRequest(Request newRequest)
{
	request = newRequest;
	setVars();
}

void Cgi::setVars()
{
	if (1 == request.getHeaders().count("Host"))
	{
		string host = request.getHeaders().at("Host");
		envVars["REMOTE_ADDR"] = host;
		size_t pos = host.find(":");
		if (pos != string::npos)
		{
			envVars["SERVER_NAME"] = host.substr(0, pos);
			envVars["SERVER_PORT"] = host.substr(pos + 1);
		}
	}

	envVars["AUTH_TYPE"] = "Basic";
	if (1 == request.getHeaders().count("Content-Length"))
		envVars["CONTENT_LENGTH"] = request.getHeaders().at("Content-Length");
	if (1 == request.getHeaders().count("Content-Type"))
		envVars["CONTENT_TYPE"] = request.getHeaders().at("Content-Type");
	envVars["DOCUMENT_ROOT"] = "./";
	envVars["GATEWAY_INTERFACE"] = "CGI/1.1";
	envVars["HTTP_COOKIE"] = "";
	envVars["PATH_INFO"] = request.getFullUriWithoutParams();
	envVars["PATH_TRANSLATED"] = ".//";
	if (!request.getParams().empty())
		envVars["QUERY_STRING"] = request.getParams();
	else
		envVars["QUERY_STRING"] = "";
	envVars["REDIRECT_STATUS"] = "200";
	envVars["REQUEST_METHOD"] = request.getMethod();
	envVars["REQUEST_URI"] = request.getFullUriWithoutParams();
	envVars["SCRIPT_FILENAME"] = request.getFullUriWithoutParams();
	envVars["SCRIPT_NAME"] = request.getFullUriWithoutParams();
	envVars["SERVER_PROTOCOL"] = "HTTP/1.1";
	envVars["UPLOAD_PATH"] = request.getUploadPath();
	envVars["UPLOAD_DIR"] = request.getUploadDir();
	envVars["PAYLOAD"] = request.stringBody();
}

void Cgi::executeBin(int &file, char **argv, char **envp)
{
	dup2(file, STDOUT_FILENO);
	close(file);

	if (-1 == execve(argv[0], argv, envp))
		throw runtime_error("Bad script");
}

void Cgi::readData()
{
	char readbuffer[80];
	string receive_output;
	while (1)
	{
		int bytes_read = read(pipeOut[0], readbuffer, sizeof(readbuffer) - 1);

		if (bytes_read <= 0)
			break;

		readbuffer[bytes_read] = '\0';
		receive_output += readbuffer;
	}
	close(pipeOut[0]);
}

void Cgi::writeData()
{
	close(pipeIn[0]);
	close(pipeOut[1]);
	string body = request.stringBody();
	if ((size_t)write(pipeIn[1], body.c_str(), body.length()) != body.length())
		exit(1);

	close(pipeIn[1]);
}

void Cgi::run()
{
	if (-1 == pipe(pipeIn))
	{
		return;
	}
	if (-1 == pipe(pipeOut))
	{
		close(pipeIn[0]);
		close(pipeIn[1]);
		return ;
	}

	pid = fork();

	if (0 == pid)
	{
		char *args[3];
		args[0] = strdup(PHPBIN);
		if (".py" == request.getExtension())
			args[0] = strdup(PYTHONBIN);
		else if (".pl" == request.getExtension())
			args[0] = strdup(PERLBIN);
		else
			args[0] = strdup(PHPBIN);
	
		args[1] = strdup(request.getFullUriWithoutParams().c_str());
		args[2] = 0;
		
		char **envp = new char *[envVars.size() + 1];

		int i = 0;
		for (map<string, string>::iterator it = envVars.begin(); it != envVars.end(); it++)
		{
			envp[i] = strdup(string(it->first + "=" + it->second).c_str());
			i++;
		}
		envp[envVars.size()] = NULL;

		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);

		close(pipeIn[0]);
		close(pipeIn[1]);
		close(pipeOut[0]);
		close(pipeOut[1]);
		execve(args[0], args, envp);
	}
	else if (this->pid > 0){}
	else
		cout << "Fork failed" << endl;
}
