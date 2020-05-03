# intercept_proxy
The C program opens a socket to proxy your browser through.  On recieving a request, send.sh lets us edit the request before it is sent using netcat.  Traffic is stored in the current directory.

# todo
add https
make the server continuously listen, either by starting a new process every time we get a connection or something else...
other tools than vim, (use prompt to do different things with the intercepted request)
