#!/usr/bin/python3

import cgi, os

method = os.environ["UPLOAD_PATH"]
form = cgi.FieldStorage()
fileitem = form['filename']

upload_dir = os.environ["UPLOAD_PATH"]

output = os.path.join(upload_dir, fileitem.filename)

if fileitem.filename:
   open(output, "wb").write(fileitem.file.read())
   message = 'The file "' + fileitem.filename + '" was uploaded to ' + upload_dir + ' successfully'
else:
   message = 'Uploading Failed'

print("<H1> " + message + " </H1>")
