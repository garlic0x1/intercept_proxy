#include "common.h"
#include <sys/mman.h>
#include <strings.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/ssl.h>



int https_req(char *host, char *request, char *ret) {

  char response[MAXLINE + 1];
  in_addr_t in_addr;
  struct protoent *protoent;
  struct hostent *hostent;
  struct sockaddr_in sockaddr_in;

  int sockfd;
  size_t n;

  unsigned short port = 443;

  // build socket
  protoent = getprotobyname("tcp");
    if (protoent == NULL) {
    perror("getprotobyname");
    exit(EXIT_FAILURE);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
  if (sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  hostent = gethostbyname(host);
  if (hostent == NULL) {
    fprintf(stderr, "error: get host %s\n", host);
    exit(EXIT_FAILURE);
  }

  //create the address as a binary stream          vector of adresses for host
  in_addr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostent->h_addr_list)));
  if (in_addr == (in_addr_t)-1) {
    fprintf(stderr, "error: inet_addr(\"%s\")\n", *(hostent->h_addr_list));
    exit(EXIT_FAILURE);
  }

  // point the socket to the address created before
  sockaddr_in.sin_addr.s_addr = in_addr;
  // define the type of connection?
  sockaddr_in.sin_family = AF_INET;
  // set the port
  sockaddr_in.sin_port = htons(port);

  //connect
  if (connect(sockfd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  // init openssl
  SSL_load_error_strings();
  SSL_library_init();
  SSL_CTX *ssl_ctx = SSL_CTX_new (SSLv23_client_method ());

  // create ssl connection and attatch to socket
  SSL *conn = SSL_new(ssl_ctx);
  SSL_set_fd(conn,sockfd);

  //perform handshake
  int err = SSL_connect(conn);
  if (err != 1) {
    perror("failed handshake\n");
    exit(EXIT_FAILURE);
  }

  printf("writing to internet:\n%s\n",request);
  SSL_write(conn, request, MAXLINE);
  SSL_read(conn, response, MAXLINE);
  printf("SSL_read():\n%s\n",response);
  strcpy(ret,response);

}

int main(int argc, char **argv) {
  int listenfd, connfd, n;
  struct sockaddr_in servaddr;
  uint8_t buff[MAXLINE+1], recvline[MAXLINE+1];

  // create a socket to listen
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    return 1;
  }

  // Setting up the address
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERVER_PORT);

  //listen and bind
  if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0 ) {
    perror("bind error");
    return 1;
  }

  if ((listen(listenfd, 10)) < 0 ) {
    perror("listen error");
    return 1;
  }

  for (int i= 0 ; ; i++) {
    printf("forloop\n");
    struct sockaddr_in addr;
    socklen_t addr_len;

    // set up file to write request to
    FILE *fp;

    // build the filename
    char filename[MAXLINE+1];
    sprintf(filename, "request%i", i);
    printf("filename is: %s\n",filename);

    // open the file to write
    fp = fopen(filename,"w");

    // accept blocks until incoming connection arrives
    // it returns file descriptor to connection
    printf("waiting for connection\n");
    //fflush(stdout);
    connfd = accept(listenfd, (SA *) NULL, NULL);

    // zero out the recieve buffer to make sure it ends up null terminated
    memset(recvline, 0, MAXLINE);
    // read client message
    while ((n = read(connfd, recvline, MAXLINE-1)) > 0) {
      printf("%s", recvline);
      fputs(recvline, fp);
      fclose(fp);

      if (recvline[n-1] == '\n') {
        break;
      }
      memset(recvline, 0, MAXLINE);
    }
    if (n < 0) {
      perror("read error");

      return 1;
    }

    //// TODO PUT SOMETHING HERE TO WAIT FOR FORWARD
    char host[64];
    gets(host);

    // open the file again to send the request to the internet
    int fd = open(filename,O_RDONLY);
    int flen = lseek(fd,0,SEEK_END);
    char *data = mmap(0, flen, PROT_READ, MAP_PRIVATE, fd, 0);

    // code to get host... for now, we will have the host entered manually.
    https_req(host, data, buff);

    printf("debug1\n");
    // now send a response

    // NOTE: normally you want to check results of write and close in case
    // errors occur
    snprintf((char*)buff, sizeof(buff), "%s", buff);
    write(connfd, (char*)buff, MAXLINE);
    //write(connfd, "aaaaaa", strlen((char*)buff));
    printf("buff:\n%s\n",buff);
    //close(connfd);
  }
}
