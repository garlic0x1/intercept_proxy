#include "include.h"

int main() {
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
    char responsef[MAXLINE+1];
    sprintf(filename, "request%i", i);
    sprintf(responsef, "response%i", i);
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

    // send the request
    printf("%s", filename);
    char command[64];
    snprintf(command,64,"./send.sh %s",filename);
    system(command);
    //execl("./send.sh", filename, NULL);

    printf("debug1\n");
    // now send a response

    fp = fopen(responsef, "r");

    sendfile(connfd, fileno(fp), NULL, MAXLINE);
  }
}
