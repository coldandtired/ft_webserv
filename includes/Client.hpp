#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"

using namespace std;

class Client
{
	private:
		time_t			lastRequestTime;
		Server			server;
		Request			request;
		Response		response;
	public:
		void	clearRequest();
		void	createResponse();
		bool	isPostTooBig() const;
		void	replaceRequestBody(string newBody);
		void	updateRequestBody(char *buffer, size_t bytesRead);
		void	updateRequestHeader(char *buffer, size_t bytesRead);

		// Getters
		const time_t		&getLastRequestTime() const;
		const Request		&getRequest() const;
		const string		&getRequestState() const;
		Response		&getResponse();
		const Server		&getServer() const;

		// Setters
		void	setServer(Server);
};

ostream &operator<<(ostream &stream, Client const &client);

#endif
