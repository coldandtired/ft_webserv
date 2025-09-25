#ifndef SERVER_HPP
# define SERVER_HPP

# include <fcntl.h>
# include <iostream>
# include <map>
# include <netinet/in.h>
# include <string>
# include <string.h>
# include <sys/epoll.h>
# include <unistd.h>

# include "Location.hpp"
# include "ServerConfig.hpp"
# include "ServerConnectionException.hpp"
# include "ServerRunningException.hpp"

class Server
{
	private:
		ServerConfig		config;
		int					listenFD;
		map<string, string>	mimeTypes;
		sockaddr_in			sockaddr;
		map<int, string>	statusCodes;
	public:
		string defaultFile(string directory) const;
		string errorFile(int code) const;
		void init();
		void initResources();
		bool isAllowed(string directory, string method);
		bool isListing(string) const;
		string mimeType(string) const;
		string redirPath(string) const;
		string statusCode(int) const;
		string uploadPath(string location) const;
		bool validCGI(string extension, string directory) const;

	// Getters
	const ServerConfig &getConfig() const;
	const int &getListenFD() const;
	const string &getRootDir() const;
	const int &getMaxClientBodySize() const;

	// Setters
	void setConfig(ServerConfig);

};

ostream &operator<<(ostream &stream, Server const &server);

#endif
