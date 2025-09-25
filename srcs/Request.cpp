#include "../includes/Request.hpp"

Request::Request()
{
	clearRequest();
}

void Request::checkHeader(char *buffer, size_t bytesRead, size_t headerBytes)
{
	if ("INCOMPLETE_BODY" == requestState)
	{
		if (bytesRead > headerBytes + 4)
		{
			char *bodyBuffer = &buffer[headerBytes + 4];
			size_t bodyLength = bytesRead - headerBytes - 4;
			if (chunked)
			{
				readChunk(bodyBuffer, bodyLength);
				return;
			}
			else
				updateBody(bodyBuffer, bodyLength);
		}
	}
}

void Request::clearRequest()
{
	body.clear();
	cgiRequest = false;
	chunk.clear();
	chunked = false;
	chunkLength = 0;
	contentLength = 0;
	directory.clear();
	extension.clear();
	fullUri.clear();;
	fullUriWithoutParams.clear();
	headerFields.clear();
	listing = false;
	method.clear();;
	params.clear();;
	requestLine.clear();
	requestState = "INCOMPLETE_HEADER";
	uri.clear();
	version.clear();
}

bool Request::expectsContinue() const
{
	if (1 == headerFields.count("Expect"))
		return "100-continue" == headerFields.at("Expect");
	return false;
}

void Request::parseHeader()
{
	vector<string> splitHeader = Utils::splitString(requestLine, '\n');
	requestLine = splitHeader[0];
	Utils::trimString(requestLine);
	vector<string> splitLine = Utils::splitString(requestLine, ' ');
	if (3 != splitLine.size())
		throw InvalidRequestException("Missing request-line");

	method = splitLine[0];

	uri = splitLine[1];
	version = splitLine[2];

	for (vector<string>::iterator it = splitHeader.begin(); it != splitHeader.end(); it++)
	{
		size_t keyPos = it->find(':');
		if (string::npos == keyPos)
			continue;
		string key = it->substr(0, keyPos);
		string value = it->substr(keyPos + 2);
		Utils::trimString(value);
		Utils::trimString(key);
		headerFields[key] = value;
	}

	requestState = "COMPLETE_HEADER";
	splitPath();

	if ("POST" == method)
	{
		if (1 == headerFields.count("Content-Length"))
		{
			contentLength = Utils::stoi(headerFields.at("Content-Length"));
			requestState = "INCOMPLETE_BODY";
		}
		else if (1 == headerFields.count("Transfer-Encoding"))
		{
			if ("chunked" == headerFields.at("Transfer-Encoding"))
			{
				chunked = true;
				requestState = "INCOMPLETE_BODY";
			}
		}
		else
			requestState = "BAD";
	}
	else
		requestState = "COMPLETE";
}

void Request::readChunk(char *buffer, size_t len)
{
	size_t limit = 0;

	if (chunk.empty())
	{
		size_t dataStart = 0;
		string tempStr;
		for (size_t i = 0; i < len - 1; i++)
		{
			tempStr += buffer[i];
			if ('\r' == buffer[i] && '\n' == buffer[i + 1])
			{
				istringstream ss(tempStr);
				ss >> hex >> chunkLength;
				contentLength += chunkLength;
				dataStart = i + 2;
				break;
			}
		}
		limit = len - dataStart >= chunkLength ? chunkLength + dataStart : len;
		writeBytes(buffer, dataStart, limit, len);
	}
	else
	{
		limit = chunkLength - chunk.size() <= len ? chunkLength - chunk.size() : len;
		writeBytes(buffer, 0, limit, len);
	}
}

void Request::splitPath()
{
	size_t pos;

	pos = uri.find_last_of(".");
	if (string::npos == pos)
	{
		isDirectory = true;
		extension = "";
		fullUri = server.getRootDir() + uri + server.defaultFile(uri);
		listing = server.isListing(uri);
		directory = server.getRootDir() + uri;
		location = uri;
		fullUriWithoutParams = fullUri;
		cgiRequest = false;
	}
	else
	{
		isDirectory = false;
		pos = uri.find_last_of("/");
	 	location = uri.substr(0, pos);
		fullUri = server.getRootDir() + uri;
		listing = false;
		pos = fullUri.find_last_of("/");
	 	directory = fullUri.substr(0, pos) + "/";
		pos = fullUri.find_first_of("?");
		params = pos != string::npos ? fullUri.substr(pos + 1) : "";
	 	fullUriWithoutParams = pos != string::npos ? fullUri.substr(0, pos) : fullUri;
		pos = fullUriWithoutParams.find_last_of(".");
		extension = fullUriWithoutParams.substr(pos);
		cgiRequest = server.getRootDir() + "/cgi-bin/" == directory;
		uploadDir = server.uploadPath(location);
		uploadPath = server.getRootDir() + "/" + server.uploadPath(location);
	}
}

string Request::stringBody() const
{
	if ("COMPLETE" != requestState)
		return "";
	
	string output = "";
	for (list<char>::const_iterator it = body.begin(); it != body.end(); it++)
		output += *it;
	
	return output;
}

void Request::updateBody(char *buffer, size_t bytesRead)
{
	if (bytesRead > 0)
	{
		if (chunked)
		{
			readChunk(buffer, bytesRead);
			return;
		}
		size_t i = 0;
		while (i < bytesRead)
		{
			body.push_back(buffer[i]);
			body2 += buffer[i];
			i++;
		}
	}
	requestState = body.size() == contentLength ? "COMPLETE" : "INCOMPLETE_BODY";
}

void Request::updateHeader(char *buffer, size_t bytesRead, Server newServer)
{
	string temp;
	for (size_t i = 0; i < bytesRead; i++)
		temp += buffer[i];
	server = newServer;
	requestLine += temp;
	size_t headerBytes = requestLine.find("\r\n\r\n");
	if (string::npos != headerBytes)
	{
		requestLine = requestLine.substr(0, headerBytes);
		parseHeader();
		checkHeader(buffer, bytesRead, temp.find("\r\n\r\n"));
	}
}

void Request::writeBytes(char *buffer, size_t start, size_t limit, size_t total)
{
	for (size_t i = start; i < limit; i++)
		chunk.push_back(buffer[i]);
	if (chunk.size() == chunkLength)
	{
		for (list<char>::iterator it = chunk.begin(); it != chunk.end(); it++)
			body.push_back(*it);
		chunk.clear();
		chunkLength = 0;
	}
	if (total >= limit + 5 && "\r\n0\r\n" == string(&buffer[limit], 5))
		requestState = "COMPLETE";
	if (total - limit > 2)
		readChunk(&buffer[total - (total - limit)], total - limit);
}

// Getters
const list<char> &Request::getBody() const { return body; }
const string &Request::getBody2() const { return body2; }
const size_t &Request::getContentLength() const { return contentLength; }
const string &Request::getDirectory() const { return directory; }
const string &Request::getExtension() const { return extension; }
const string &Request::getFullUri() const { return fullUri; }
const string &Request::getFullUriWithoutParams() const { return fullUriWithoutParams; }
const map<string, string> &Request::getHeaders() const { return headerFields; }
const bool &Request::isCGIRequest() const { return cgiRequest; }
const bool &Request::isChunked() const { return chunked; }
const bool &Request::isDir() const { return isDirectory; }
const bool &Request::isListing() const { return listing; }
const string &Request::getLocation() const { return location; }
const string &Request::getMethod() const { return method; }
const string &Request::getParams() const { return params; }
const string &Request::getRequestLine() const { return requestLine; }
const string &Request::getRequestState() const { return requestState; }
const string &Request::getUploadPath() const { return uploadPath; }
const string &Request::getUploadDir() const { return uploadDir; }
const string &Request::getUri() const { return uri; }
const string &Request::getVersion() const { return version; }

void Request::setBody(string newBody)
{
	body.clear();
	for (string::iterator it = newBody.begin(); it != newBody.end(); it++)
		body.push_back(*it);
}

// Setters
void Request::setUri(string newURI)
{
	uri = newURI;
	splitPath();
}

ostream	&operator<<(ostream &stream, Request const &request)
{
	stream << "Request:" << endl
		<< "  CGI?: " << (request.isCGIRequest() ? "yes" : "no") << endl
		<< "  Chunked?: " << (request.isChunked() ? "yes" : "no") << endl
		<< "  Content length: " << request.getContentLength() << endl
		<< "  Directory: " << request.getDirectory() << endl
		<< "  Extension: " << request.getExtension() << endl
		<< "  Full URI: " << request.getFullUri() << endl
		<< "  Full URI (no params): " << request.getFullUriWithoutParams() << endl
		<< "  Is dir?: " << (request.isDir() ? "yes" : "no") << endl
		<< "  Listing?: " << (request.isListing() ? "yes" : "no") << endl
		<< "  Location: " << request.getLocation() << endl
		<< "  Method: " << request.getMethod() << endl
		<< "  Params: " << request.getParams() << endl
		<< "  Request Line: " << request.getRequestLine() << endl
		<< "  State: " << request.getRequestState() << endl
		<< "  Upload Path: " << request.getUploadPath() << endl
		<< "  URI: " << request.getUri() << endl
		<< "  Headers: " << endl;
	for (map<string, string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); it++)
		stream << "    " << it->first << " : " << it->second << endl;

	return stream;
}
