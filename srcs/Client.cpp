#include "../includes/Client.hpp"

void Client::clearRequest()
{
	request.clearRequest();
}

void Client::createResponse()
{
	response.setRequest(request);
	response.setServer(server);
	response.makeResponse();
}

bool Client::isPostTooBig() const
{
	if ("POST" != request.getMethod())
		return false;
	size_t max = server.getMaxClientBodySize();
	return request.getContentLength() > max;
}

void Client::updateRequestHeader(char *buffer, size_t bytesRead)
{
	lastRequestTime = time(NULL);
	request.updateHeader(buffer, bytesRead, server);
}

void Client::updateRequestBody(char *buffer, size_t bytesRead)
{
	lastRequestTime = time(NULL);
	request.updateBody(buffer, bytesRead);
}

void Client::replaceRequestBody(string newBody)
{
	request.setBody(newBody);
}

// Getters
const time_t &Client::getLastRequestTime() const { return lastRequestTime; }
const Request &Client::getRequest() const { return request; }
const string &Client::getRequestState() const { return request.getRequestState(); }
Response &Client::getResponse() { return response; }
const Server &Client::getServer() const { return server; }

// Setters
void Client::setServer(Server newServer) { server = newServer; }

ostream	&operator<<(ostream &stream, Client const &client)
{
	stream << "Client:" << endl
		<< client.getServer() << endl;

	return stream;
}
