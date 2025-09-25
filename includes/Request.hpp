#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include "InvalidRequestException.hpp"
#include "Server.hpp"
#include "Utils.hpp"

using namespace std;

class Request
{
	private:
		list<char>				body;
		string					body2;
		bool					cgiRequest;
		list<char>				chunk;
		bool					chunked;
		size_t					chunkLength;
		size_t					contentLength;
		string					directory;
		string					extension;
		string					fullUri;
		string					fullUriWithoutParams;
		map<string, string>		headerFields;
		bool					isDirectory;
		bool					listing;
		string					location;
		string					method;
		string					params;
		string					requestLine;
		string					requestState;
		Server					server;
		string					uploadDir;
		string					uploadPath;
		string					uri;
		string					version;

		void checkHeader(char *buffer, size_t bytesRead, size_t headerBytes);
		void parseHeader();
		void readChunk(char *buffer, size_t len);
		void writeBytes(char *buffer, size_t start, size_t limit, size_t total);

	public:
		Request();
		void	clearRequest();
		bool	expectsContinue() const;
		void	splitPath();
		string	stringBody() const;
		void	updateBody(char *buffer, size_t bytesRead);
		void	updateHeader(char *buffer, size_t bytesRead, Server server);

		// Getters
		const list<char>			&getBody() const;
		const string				&getBody2() const;
		const size_t				&getContentLength() const;
		const string				&getDirectory() const;
		const string				&getExtension() const;
		const string				&getFullUri() const;
		const string				&getFullUriWithoutParams() const;
		const map<string, string>	&getHeaders() const;
		const bool					&isCGIRequest() const;
		const bool					&isChunked() const;
		const bool					&isDir() const;
		const bool					&isListing() const;
		const string				&getLocation() const;
		const string				&getMethod() const;
		const string				&getParams() const;
		const string				&getRequestLine() const;
		const string				&getRequestState() const;
		const string				&getUploadPath() const;
		const string				&getUploadDir() const;
		const string				&getUri() const;
		const string				&getVersion() const;

		// Setters
		void 	setBody(string newBody);
		void	setUri(string);
};

ostream &operator<<(ostream &stream, Request const &request);

#endif
