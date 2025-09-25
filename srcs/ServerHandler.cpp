#include "../includes/ServerHandler.hpp"

bool quitting = false;

void enable_ctrl_c(int sig)
{
	(void)sig;
	quitting = true;
}

void ServerHandler::acceptClient(int fd)
{
	int clientFD;
	struct sockaddr_in client_address;
	long client_address_size = sizeof(client_address);

	Server server = servers.at(fd);
	clientFD = accept(server.getListenFD(), (struct sockaddr *)&client_address, (socklen_t *)&client_address_size);

	if (-1 == clientFD)
	{
		cerr << "Accept problem" << endl;
		return;
	}

	if (-1 == fcntl(clientFD, F_SETFL, O_NONBLOCK))
	{
		cerr << "No-block problem" << endl;
		close(clientFD);
		return;
	}

	Client client;
	client.setServer(server);
	addToIncoming(clientFD);

	clients[clientFD] = client;
}

void ServerHandler::addToIncoming(const int fd)
{
	FD_SET(fd, &incoming);
	if (fd > lastFD)
		lastFD = fd;
}

void ServerHandler::addToOutgoing(const int fd)
{
	FD_SET(fd, &outgoing);
	if (fd > lastFD)
		lastFD = fd;
}

void ServerHandler::cleanup()
{
	cout << endl << "Quitting..." << endl;
}

void ServerHandler::closeClient(const int fd)
{
	if (FD_ISSET(fd, &incoming))
		removeFromIncoming(fd);
	if (FD_ISSET(fd, &outgoing))
		removeFromOutgoing(fd);
	close(fd);
	clients.erase(fd);
}

void ServerHandler::errorCleanup(string message)
{
	cerr << message << endl;
	exit(1);
}

void ServerHandler::handleRequest(int fd)
{
	char buffer[BUFFER_SIZE];
	int bytes_read = read(fd, buffer, BUFFER_SIZE);
	if (-1 == bytes_read)
	{
		cerr << "Read error on socket " << fd << endl;
		closeClient(fd);
		return;
	}
	if (0 == bytes_read)
	{
		cout << "Closed socket " << fd << endl;
		closeClient(fd);
		return;
	}
	else
		try
		{
			Client &client = clients.at(fd);
			string state = client.getRequestState();

			if ("INCOMPLETE_HEADER" == state)
			{
				client.updateRequestHeader(buffer, bytes_read);
				memset(buffer, 0, sizeof(buffer));
			}
			else if ("INCOMPLETE_BODY" == state)
				client.updateRequestBody(buffer, bytes_read);
			else if ("INCOMPLETE_HEADER" == state)
				client.updateRequestHeader(buffer, bytes_read);
			if ("COMPLETE" == client.getRequestState())
			{
				client.createResponse();
				cout << "[socket " << fd << "] " << client.getRequest().getUri() << " -> " << client.getResponse().getCode() << endl;
				if (client.getRequest().isCGIRequest() && 200 == client.getResponse().getCode())
				{	
					client.getResponse().getCgi().setRequest(client.getRequest());
					addToOutgoing(client.getResponse().getCgi().pipeIn[1]);
					addToIncoming(client.getResponse().getCgi().pipeOut[0]);
				}
				addToOutgoing(fd);
			}
		}
		catch (const exception &e)
		{
			cerr << "Error: " << e.what() << endl;
		}
}

void ServerHandler::init(vector<ServerConfig> configs)
{
	FD_ZERO(&incoming);
	FD_ZERO(&outgoing);
	lastFD = 0;

	for (vector<ServerConfig>::iterator it = configs.begin(); it != configs.end(); it++)
	{
		Server server;
		server.setConfig(*it);
		server.init();
		servers[server.getListenFD()] = server;
		addToIncoming(server.getListenFD());
		lastFD = server.getListenFD();
		cout << "[" << it->getHost() << ":" << it->getPort() << "] is listening" << endl;
	}
}

void ServerHandler::readCgi(int fd)
{
	char buffer[BUFFER_SIZE * 2];
	int bytes_read;
	Client &client = clients.at(fd);

	bytes_read = read(client.getResponse().getCgi().pipeOut[0], buffer, BUFFER_SIZE * 2);

	if (bytes_read == 0)
	{
		removeFromIncoming(client.getResponse().getCgi().pipeOut[0]);
		close(client.getResponse().getCgi().pipeIn[0]);
		close(client.getResponse().getCgi().pipeOut[0]);
		int status;
		waitpid(client.getResponse().getCgi().pid, &status, 0);
		return;
	}
	else if (bytes_read < 0)
	{
		removeFromIncoming(client.getResponse().getCgi().pipeOut[0]);
		close(client.getResponse().getCgi().pipeIn[0]);
		close(client.getResponse().getCgi().pipeOut[0]);
		return;
	}
	else
	{
		client.getResponse().content.append(buffer, bytes_read);
		memset(buffer, 0, sizeof(buffer));

		Response response = client.getResponse();
		response.setContent();

		client.clearRequest();
		clients[fd] = client;
		removeFromOutgoing(fd);
		removeFromIncoming(client.getResponse().getCgi().pipeOut[0]);
		send(fd, response.getContent().c_str(), response.getContent().length(), 0);
	}
}

void ServerHandler::removeFromIncoming(const int fd)
{
	FD_CLR(fd, &incoming);
	if (fd == lastFD)
		lastFD--;
}

void ServerHandler::removeFromOutgoing(const int fd)
{
	FD_CLR(fd, &outgoing);
	if (fd == lastFD)
		lastFD--;
}

void ServerHandler::run()
{
	fd_set incomingFDs;
	fd_set outgoingFDs;
	struct timeval timer;

	signal(SIGINT, enable_ctrl_c);

	while (true)
	{
		timer.tv_sec = 1;
		timer.tv_usec = 0;
		incomingFDs = incoming;
		outgoingFDs = outgoing;

		if (-1 == select(lastFD + 1, &incomingFDs, &outgoingFDs, NULL, &timer))
		{
			if (quitting)
			{
				cleanup();
				return;
			}
			errorCleanup("Select error");
		}

		for (int i = 0; i <= lastFD; ++i)
		{
			if (FD_ISSET(i, &incomingFDs) && servers.count(i))
				acceptClient(i);
			else if (FD_ISSET(i, &incomingFDs) && clients.count(i))
				handleRequest(i);
			else if (FD_ISSET(i, &outgoingFDs) && clients.count(i))
			{
				Client client = clients.at(i);
				if (client.getRequest().isCGIRequest() && 200 == client.getResponse().getCode())
				{
					if (FD_ISSET(client.getResponse().getCgi().pipeIn[1], &outgoingFDs))
						sendCGI(i);
					else if (FD_ISSET(client.getResponse().getCgi().pipeOut[0], &incomingFDs))
					{
						removeFromIncoming(client.getResponse().getCgi().pipeOut[0]);
						readCgi(i);
					}
				}
				else
					sendNormalResponse(i);
			}
		}
	}
}

void ServerHandler::sendCGI(int fd)
{
	int bytes_sent;
	Client &client = clients.at(fd);
	string ss = client.getRequest().stringBody();

	if (ss.length() == 0)
		bytes_sent = 0;
	else if (ss.length() >= BUFFER_SIZE)
		bytes_sent = write(client.getResponse().getCgi().pipeIn[1], ss.c_str(), BUFFER_SIZE);
	else
		bytes_sent = write(client.getResponse().getCgi().pipeIn[1], ss.c_str(), ss.length());

	if (-1 == bytes_sent)
	{
		removeFromOutgoing(client.getResponse().getCgi().pipeIn[1]);
		close(client.getResponse().getCgi().pipeIn[1]);
		close(client.getResponse().getCgi().pipeOut[1]);
	}
	else if (bytes_sent == 0 || (size_t)bytes_sent == ss.size())
	{
		removeFromOutgoing(client.getResponse().getCgi().pipeIn[1]);
		close(client.getResponse().getCgi().pipeIn[1]);
		close(client.getResponse().getCgi().pipeOut[1]);
	}
	else
	{
		string newBody = client.getRequest().stringBody().substr(bytes_sent);
		client.replaceRequestBody(newBody);
	}
}

void ServerHandler::sendNormalResponse(int fd)
{
	Client &client = clients.at(fd);
	Response response = client.getResponse();
	clients[fd] = client;
	send(fd, response.getContent().c_str(), response.getContent().length(), 0);
	removeFromOutgoing(fd);
	client.clearRequest();
	closeClient(fd);
}

void ServerHandler::start(vector<ServerConfig> configs)
{
	init(configs);
	run();
}
