#include "../includes/Response.hpp"

/*
	Returns a basic default error page if not provided in the config file
*/
void Response::generateDefaultErrorPage()
{
	string text = "<!DOCTYPE html>"
				  "<html lang=\"en\">"
				  "<head>"
				  "<meta charset=\"UTF-8\">"
				  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
				  "<title>Error " +
				  Utils::intToString(code) + "</title>"
											 "</head>"
											 "<body>"
											 "<h1>Something went wrong</h1>" +
											 "<h2>" + 
				  Utils::intToString(code) + " " + server.statusCode(code) +  "</h2>"
											 "</body>"
											 "</html>";											
	contentLength = text.length();
	content = content + "Content-Length: " + Utils::intToString(contentLength) + "\r\n\r\n";
	content += text;
}

/*
	Turns the requested file extension into its mimetype
*/
string Response::getMimeType() const
{
	size_t pos = filename.find_last_of(".");
	string extension = pos != string::npos ? filename.substr(pos) : "";

	return server.mimeType(extension);
}

/*
	Formats the time of the response for the header
*/
string Response::getTime() const
{
	char buffer[80];
	time_t now = time(NULL);
	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S %z", localtime(&now));
	return string(buffer);
}

string Response::getUploadFilename()
{
	//cerr << "here1" << endl;
	if (request.isChunked())
	{
		if (0 == request.getHeaders().count("Filename"))
			return "MISSING";
		return request.getDirectory() + "/" + request.getHeaders().at("Filename");
	}

	list<char> input = request.getBody();
	string temp = "";
	for (list<char>::iterator it = input.begin(); it != input.end(); it++)
		temp += *it;

	temp += "\0";

	//cerr << "here2" << endl;

	string toFind = "Content-Disposition: form-data; name=\"filename\"; filename=\"";
	int pos = temp.find(toFind) + toFind.length();
	int pos2 = temp.substr(pos).find_first_of('"');
	return request.getDirectory() + "/" + temp.substr(pos, pos2);
}

/*
	Handles a DELETE request and returns confirmation or an error
*/
void Response::handleDelete()
{
	contentType = "html/text";

	//if folder...
	if (request.isDir())
	{
		code = 403;
		contentType = "text/html";
		makeHeader();
		makeError();
		return;
	}

	if (0 == remove(request.getFullUri().c_str()))
	{
		code = 200;
		makeHeader();
		string text = "<!DOCTYPE html>"
					  "<html lang=\"en\">"
					  "<head>"
					  "<meta charset=\"UTF-8\">"
					  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
					  "<title>Deleted!</title>"
					  "</head>"
					  "<body>"
					  "<h1>Deleted " +
					  request.getUri() + "</h1>"
								 "</body>"
								 "</html>";
		contentLength = text.length();
		content = content + "Content-Length: " + Utils::intToString(contentLength) + "\r\n\r\n";
		content += text;
	}
	else
	{
		code = 404;
		makeHeader();
		makeError();
	}
}

/*
	Handles a GET request, checking whether we should serve the normal content or a list of files
*/
void Response::handleGet()
{
	if (string::npos != request.getFullUriWithoutParams().find_last_of("."))
	{
		ifstream file(request.getFullUriWithoutParams().c_str());
		if (!file.good())
		{
			code = 404;
			contentType = "text/html";
			makeHeader();
			makeError();
			return;
		}
	}

	if (request.isCGIRequest())
	{
		code = 200;
		if (pipe(cgifd) < 0)
		{
			cout << "Pipe error" << endl;
			return;
		}
		content.clear();
		cgi.setRequest(request);
		cgi.run();
		contentType = "text/html";
		return;
	}

	setCode();
	makeHeader();

	if (request.isListing())
		makeListing();
	else
		makeContent();
}

/*
	Handles a POST request
*/
void Response::handlePost()
{
	// Upload size too big
	size_t max = server.getMaxClientBodySize();
	if (request.getContentLength() > max)
	{
		code = 413;
		contentType = "text/html";
		makeHeader();
		makeError();
		return;
	}

	if ("/cgi-bin" == request.getLocation())
	{
		// Location has an upload path set
		string uploadPath = server.uploadPath(request.getLocation());
		if (uploadPath.empty())
		{
			code = 403;
			contentType = "text/html";
			makeHeader();
			makeError();
			return;
		}

		code = 200;
		if (pipe(cgifd) < 0)
		{
			cout << "Pipe error" << endl;
			return;
		}
		content.clear();
		cgi.setRequest(request);

		cgi.run();
		return;
	}
	
	string fn = getUploadFilename();
	if ("MISSING" == fn)
	{
		code = 400;
		contentType = "text/html";
		makeHeader();
		makeError();
		return;
	}

	list<char> chars = request.isChunked() ? request.getBody() : removeBoundary();

	std::ofstream fs(fn.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	for (list<char>::iterator it = chars.begin(); it != chars.end(); it++)
			fs.write(&(*it), 1);
	fs.close();
	code = 201;
	makeHeader();
	string text = "<!DOCTYPE html>"
					"<html lang=\"en\">"
					"<head>"
					"<meta charset=\"UTF-8\">"
					"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
					"<title>Uploaded!</title>"
					"</head>"
					"<body>"
					"<h1>Uploaded " +
					fn + "</h1>"
								"</body>"
								"</html>";
	contentLength = text.length();
	content = content + "Content-Length: " + Utils::intToString(contentLength) + "\r\n\r\n";
	content += text;
}

/*
	Appends the content to the previously-created header. Either an error page or the requested file
*/
void Response::makeContent()
{
	if (200 != code && 301 != code)
	{
		contentType = "text/html";
		makeError();
	}
	else
	{
		contentType = getMimeType();
		ifstream t(filename.c_str());
		stringstream buffer;
		buffer << t.rdbuf();
		contentLength = buffer.str().length();
		content = content + "Content-Length: " + Utils::intToString(contentLength) + "\r\n\r\n";
		content += buffer.str();
	}
}

/*
	Returns either an error page from the config or a generated one
*/
void Response::makeError()
{
	struct stat s;
	request.setUri(server.errorFile(code));
	if (0 == stat(request.getFullUri().c_str(), &s))
	{
		if (s.st_mode & S_IFDIR)
		{
			generateDefaultErrorPage();
			return;
		}
	}

	ifstream file(request.getFullUri().c_str());
	if (!file.good())
		generateDefaultErrorPage();
	else
	{
		stringstream buffer;
		buffer << file.rdbuf();
		contentLength = buffer.str().length();
		content = content + "Content-Length: " + Utils::intToString(contentLength) + "\r\n\r\n";
		content += buffer.str();
	}
}

/*
	Makes the basic header (time, version, return code, content-type)
*/
void Response::makeHeader()
{
	content = "HTTP/1.1 " + Utils::intToString(code) + " " + server.statusCode(code) + "\r\n" + "Content-Type: " + contentType + "\r\n" + "Date: " + getTime() + "\r\n";
}

/*
	Returns a list of files/directories in the request location
*/
void Response::makeListing()
{
	DIR *root;
	dirent *current;
	string test = "";
	root = opendir(request.getDirectory().c_str());

	while ((current = readdir(root)) != NULL)
	{
		if (current->d_name == string(".") || current->d_name == string(".."))
			continue;
		test.append(current->d_name).append("\n");
	}

	closedir(root);
	contentLength = test.length();
	content = content + "Content-Length: " + Utils::intToString(contentLength) + "\r\n\r\n";
	content += test;
}

/*
	The entry method of the class
*/
void Response::makeResponse()
{
	string redir = server.redirPath(request.getLocation());
	if (!redir.empty())
	{
		request.setUri(redir);
		redirected = true;
	}
	else
		redirected = false;

	// Location has the right GET, POST, etc.
	if (!server.isAllowed(request.getLocation(), request.getMethod()))
	{
		code = 405;
		contentType = "text/html";
		makeHeader();
		makeError();
		return;
	}

	// If it's the cgi directory, check the extension is allowed
	if (request.isCGIRequest() && !server.validCGI(request.getExtension(), request.getLocation()))
	{
		code = 400;
		contentType = "text/html";
		makeHeader();
		makeError();
		return;
	}

	filename = request.getFullUriWithoutParams();

	contentType = getMimeType();

	if ("DELETE" == request.getMethod())
		handleDelete();
	else if ("GET" == request.getMethod())
		handleGet();
	else if ("POST" == request.getMethod())
		handlePost();
}

list<char> Response::removeBoundary()
{
	list<char> input = request.getBody();
	list<char> output;
	string temp = "";
	for (list<char>::iterator it = input.begin(); it != input.end(); it++)
		temp += *it;
	string toFind = "boundary=";
	string temp2 = request.getHeaders().at("Content-Type");
	int pos = temp2.find(toFind) + toFind.length();
	int pos2 = temp2.substr(pos).find("\r\n");
	string boundary = "--" + temp2.substr(pos, pos2);

	int pos3 = temp.find("\r\n\r\n") + 4;
	int pos4 = temp.substr(pos3).find(boundary) - 2;
	string body = temp.substr(pos3, pos4);

	for (string::iterator it = body.begin(); it != body.end(); it++)
		output.push_back(*it);

	return output;
}

/*
	Sets the return code to ok or not found. This is overridden elsewhere
*/
void Response::setCode()
{
	ifstream file(request.getFullUriWithoutParams().c_str());
	if (file.good())
		code = redirected ? 301 : 200;
	else
	{
		code = 404;
		contentType = "text/html";
	}
}

// Getters
Cgi		&Response::getCgi() { return cgi; }
const int		&Response::getCode() const { return code; }
const string	&Response::getContent() const { return content; }
const int		&Response::getContentLength() const { return contentLength; }
const string	&Response::getContentType() const { return contentType; }
const string	&Response::getFilename() const { return filename; }
const Request	&Response::getRequest() const { return request; }

// Setters
void	Response::setRequest(Request newRequest) { request = newRequest; }
void	Response::setServer(Server newServer) { server = newServer; }

ostream	&operator<<(ostream &stream, Response const &response)
{
	stream << "Response:" << endl
		<< "  Code: " << response.getCode() << endl
		<< "  Content length: " << response.getContentLength() << endl
		<< "  Content type: " << response.getContentType() << endl
		<< "  Filename: " << response.getFilename() << endl
		<< "  Uri: " << response.getRequest().getFullUri() << endl;
		stream << "  Body: " << response.getContent() << endl;
	return stream;
}

void Response::setContent()
{
	string oldContent = content;
	contentType = "text/html";
	makeHeader();
	content += "Content-Length: " + Utils::intToString(oldContent.length()) + "\r\n\r\n";
	content += oldContent;
}
