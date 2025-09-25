#ifndef SERVERHANDLER_HPP
# define SERVERHANDLER_HPP

# include "webserv.hpp"
# include "Client.hpp"
# include "Response.hpp"
# include "Server.hpp"

using namespace std;

class ServerHandler
{        
    private:
        map<int, Server>	servers;
        map<int, Client>	clients;
        fd_set				incoming;
        fd_set				outgoing;
        int					lastFD;

		void acceptClient(int fd);
		void addToIncoming(const int fd);
		void addToOutgoing(const int fd);
		void closeClient(const int fd);
		void fillResources();
        void handleRequest(int fd);
        void init(vector<ServerConfig> configs);
		void readCgi(int fd);
		void removeFromIncoming(const int fd);
		void removeFromOutgoing(const int fd);
		void run();
		void sendCGI(int fd);
		void sendNormalResponse(int fd);

        // void checkTimeout();

		public:
			void cleanup();
			void errorCleanup(string message);
			void start(vector<ServerConfig> configs);
};

#endif 
