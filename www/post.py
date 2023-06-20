#!/usr/bin/env python3

import cgi 


form = cgi.FieldStorage()

name  = form.getvalue('name')
email  = form.getvalue('email')
message  = form.getvalue('message')


# Send a response back to the client
# print('HTTP/1.1 200 OK\n\n')
print("<html><head><title>POST Request Example</title></head><body style='background-color:rgb(44, 0, 116)';>")
print("<h1>POST Request Example</h1>")
print("<p>Received form data:</p>")
print("<ul>")
print(f"<li>Name: {name}</li>")
print(f"<li>Email: {email}</li>")
print(f"<li>Message: {message}</li>")
print("</ul>")
print("</body></html>")