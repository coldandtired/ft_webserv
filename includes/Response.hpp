#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <dirent.h>
# include <iostream>
# include <string>

# include "Cgi.hpp"
# include "Location.hpp"
# include "Request.hpp"
# include "Server.hpp"
# include "webserv.hpp"

using namespace std;

class Response
{
	private:
		Cgi				cgi;
		int				code;
		int				contentLength;
		string			contentType;
		string			filename;
		bool			redirected;
		Request			request;
		Server			server;

		void			generateDefaultErrorPage();
		string			getMimeType() const;
		string			getTime() const;
		string			getUploadFilename();
		void			handleDelete();
		void			handleGet();
		void			handlePost();
		void			makeError();
		void			makeListing();
		list<char>		removeBoundary();
		void			setCode();

	public:
		void makeResponse();
		string			content;
		int				cgifd[2];
		void			makeHeader();
		void			makeContent();
		void			setContent();

		// Getters
		Cgi					&getCgi();
		const int			&getCode() const;
		const string		&getContent() const;
		const int			&getContentLength() const;
		const string		&getContentType() const;
		const string		&getFilename() const;
		const Request		&getRequest() const;

		// Setters
		void setServer(Server);
		void setRequest(Request);
};

ostream &operator<<(ostream &stream, Response const &response);

#endif
