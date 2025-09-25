# include <iostream>
# include <map>
# include <string>
# include <sstream>
# include <stdexcept>
# include <vector>

# include "../includes/ConfigException.hpp"
# include "../includes/Parse.hpp"
# include "../includes/Request.hpp"
# include "../includes/ServerConfig.hpp"
# include "../includes/Utils.hpp"
# include "../includes/ServerHandler.hpp"

using namespace std;

void printParser(Parse &parser)
{
	const vector<Parse::ServerBlock> &serverBlocks = parser.getServerBlocks();

	vector<Parse::ServerBlock>::const_iterator serverIt; // Iterate through the server blocks (all server blocks)
	for (serverIt = serverBlocks.begin(); serverIt != serverBlocks.end(); ++serverIt)
	{
		const Parse::ServerBlock &server = *serverIt;
		cout << "----- Server Block --------------" << endl;

		cout << "Port: " << server._port << endl;
		cout << "Server Name: " << server._server_name << endl;
		cout << "Host: " << server._host << endl;
		cout << "Root Directory: " << server._root_directory << endl;
		cout << "Client Max Body Size: " << server._client_max_body_size << endl;

		cout << "Error Pages:" << endl;
		for (map<int, string>::const_iterator errorPageIt = server._error_pages.begin(); errorPageIt != server._error_pages.end(); ++errorPageIt)
		{
			cout << "  Error Code: " << errorPageIt->first << " Path: " << errorPageIt->second << endl;
		}

		// location blocks

		cout << "Locations:" << endl;
		for (map<string, vector<string> >::const_iterator routeIt = server._allowed_methods.begin(); routeIt != server._allowed_methods.end(); ++routeIt)
		{
			cout << "  Path: " << routeIt->first << endl;
			cout << "  Allowed Methods: ";
			for (vector<string>::const_iterator methodIt = routeIt->second.begin(); methodIt != routeIt->second.end(); ++methodIt)
			{
				cout << *methodIt << " ";
			}
			cout << endl;
		}

		cout << "Location-specific Roots:" << endl;
		for (map<string, string>::const_iterator locRootIt = server._location_roots.begin(); locRootIt != server._location_roots.end(); ++locRootIt)
		{
			cout << "  Path: " << locRootIt->first << " Root: " << locRootIt->second << endl;
		}

		cout << "Autoindex:" << endl;
		for (map<string, string>::const_iterator autoIt = server._autoindex.begin(); autoIt != server._autoindex.end(); ++autoIt)
		{
			cout << "  Path: " << autoIt->first << " Root: " << autoIt->second << endl;
		}

		cout << "Upload Paths:" << endl;
		for (map<string, string>::const_iterator uploadPathIt = server._upload_paths.begin(); uploadPathIt != server._upload_paths.end(); ++uploadPathIt)
		{
			cout << "  Path: " << uploadPathIt->first << " Upload Path: " << uploadPathIt->second << endl;
		}

		cout << "CGI Paths:" << endl;
		for (map<string, string>::const_iterator cgiPathIt = server._cgi_paths.begin(); cgiPathIt != server._cgi_paths.end(); ++cgiPathIt)
		{
			cout << "  Path: " << cgiPathIt->first << " CGI Path: " << cgiPathIt->second << endl;
		}

		cout << "Default files:" << endl;
		for (map<string, string>::const_iterator defaultFilesIt = server._defaultfile.begin(); defaultFilesIt != server._defaultfile.end(); ++defaultFilesIt)
		{
			cout << "  Path: " << defaultFilesIt->first << " Default file: " << defaultFilesIt->second << endl;
		}

		cout << "Redirects:" << endl;
		for (map<string, string>::const_iterator redirectIt = server._returndir.begin(); redirectIt != server._returndir.end(); ++redirectIt)
		{
			cout << "  Path: " << redirectIt->first << " To: " << redirectIt->second << endl;
		}

		cout << "CGI Extensions:" << endl;
		for (map<string, vector<string> >::const_iterator cgiExtIt = server._cgi_extensions.begin(); cgiExtIt != server._cgi_extensions.end(); ++cgiExtIt)
		{
			cout << "  Path: " << cgiExtIt->first << " Extensions: ";
			for (vector<string>::const_iterator extIt = cgiExtIt->second.begin(); extIt != cgiExtIt->second.end(); ++extIt)
			{
				cout << *extIt << " ";
			}
			cout << endl;
		}

		cout << "------------------------" << endl;
	}
}

int main(int ac, char **av, char **env)
{
	if ("./webserv" != string(av[0]))
	{
		cerr << "Run from the root directory!" << endl;
		return 1;
	}

	// Gets the absolute path
	string path;
	int i = -1;
	while (env[++i])
		if (0 == Utils::strncmp("PWD=", env[i], 4))
			path = env[i];
	path = path.substr(4) + "/";

	try
	{
		if (ac > 2)
			throw runtime_error("error arguments | ./webserver [./path/filename.config]");
		Parse parser;

		if (ac == 1)
			parser.loadConfig("./config/default.config"); // default config file
		else
			parser.loadConfig(av[1]);

		const vector<Parse::ServerBlock> &serverBlocks = parser.getServerBlocks(); // struct _ServerBlocks

		vector<ServerConfig> configs;
		for (vector<Parse::ServerBlock>::const_iterator it = serverBlocks.begin(); it != serverBlocks.end(); it++)
		{
			ServerConfig serverConfig(path, *it);
			configs.push_back(serverConfig);
		}

		if (!configs.empty())
		{
			ServerHandler serverHandler;
			serverHandler.start(configs);
		}

		return 0;
	}
	catch (const exception &e)
	{
		cerr << "Error: " << e.what() << endl;
		return 1;
	}
}
