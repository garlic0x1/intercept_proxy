#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include <stdarg.h>
#include <strings.h>
#include <stddef.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define SERVER_PORT 8888
#define MAXLINE 12000
#define SA struct sockaddr

#endif
