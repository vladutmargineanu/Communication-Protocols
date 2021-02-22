/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * server.c
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define BUFLEN 1500

int main(int argc, char *argv[]) {
    int listenfd = 0, connfd = 0, clientfd = 0, clientfd1, clientfd2;
    char buff[BUFLEN];
    struct sockaddr_in serveraddr, clientaddr;

    if (argc != 3) {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // deschidere socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0) {
      perror("socket");
      exit(-1);
    }

    // completare informatii despre adresa serverului
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[2]));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("sin_port: %d\n", serveraddr.sin_port);

    // legare proprietati de socket
    int rc = bind(listenfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));

    if (rc < 0) {
      perror("bind");
      exit(-1);
    }

    // ascultare de conexiuni
    int enable = 1;
    rc = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    if (rc < 0) {
      perror("setsockopt");
      exit(-1);
    }
    rc = listen(listenfd, 5);

    // acceptarea unei conexiuni, primirea datelor, trimiterea raspunsului
    socklen_t clen = sizeof(clientaddr);

    clientfd1 = accept(listenfd, (struct sockaddr*) &clientaddr, &clen);
    if (clientfd1 < 0) {
        perror("accept");
    }
    printf("clientfd1");
    clientfd2 = accept(listenfd, (struct sockaddr *)&clientaddr, &clen);
    if (clientfd2 < 0) {
        perror("accept");
    }
    printf("clientfd1");
    while (1) {
      rc = recv(clientfd1, buff, sizeof(buff), 0);

      if (rc == 0) {
          fprintf(stderr, "S-a deconectat 1");
          close(clientfd1);
          close(clientfd2);
      }

      send(clientfd2, buff, sizeof(buff), 0);

      rc = recv(clientfd2, buff, sizeof(buff), 0);

      if (rc == 0) {
          fprintf(stderr, "S-a deconectat 2");
          close(clientfd1);
          close(clientfd2);
      }
      send(clientfd1, buff, sizeof(buff), 0);
    }

    // inchidere socket(i)
    close(listenfd);
    return 0;
}
