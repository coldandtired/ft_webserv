import os, sys

firstsplit = os.environ['QUERY_STRING'].split('&')
split2 = firstsplit[0].split('=')

if os.environ['REQUEST_METHOD'] == 'GET':

	print("<html>")
	print("<head>")
	print("<title>Python GET</title>")
	print("</head>")
	print("<body>")
	print("<h1>Python CGI</h1>")
	print("You selected " + split2[1])
	print("</body>")
	print("</html>")

sys.exit(0)
