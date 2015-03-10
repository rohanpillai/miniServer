#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <http.h>
#include <unistd.h>

#define PORT "4040"
#define BACKLOG 5

int main() {
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int status = getaddrinfo(NULL, PORT, &hints, &res);
  if (status != 0) {
    printf("Error: %s\n", gai_strerror(status));
    return 1;
  }

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  int binding_status = bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (binding_status == -1) {
    printf("Error trying to bind socket\n");
    return 2;
  }

  int listen_status = listen(sockfd, BACKLOG);
  if (listen_status == -1) {
    printf("Error trying to listen\n");
    return 3;
  }

  while (1) {
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int new_sockfd = accept(sockfd, (struct sockaddr*) &client_addr, &addr_size);
    if (new_sockfd < 0) {
      cout << "Error on accept\n";
      exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0) {
      cout << "Error on fork\n";
      exit(EXIT_FAILURE);
    }
    if (pid == 0) {
      close(sockfd);
      HTTP *http = new HTTP(new_sockfd);
      delete http;
      exit(0);
    } else {
      close(new_sockfd);
    }
  }
  return 0;
}
      
