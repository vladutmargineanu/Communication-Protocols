#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFLEN 256

void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    fd_set read_fds;	  // multimea de citire folosita in select()
    fd_set tmp_fds;	    // multimea de citire auxiliara( ptr select care modifica lista primita)
    int fdmax;          // valoarea maxima a file descriptorului din multimea read_fds

    char buffer[BUFLEN];
    if (argc < 3) {
       fprintf(stderr,"Usage %s server_address server_port\n", argv[0]);
       exit(0);
    }

    // Ne asiguram ca multimile de citire sunt initial vide
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &serv_addr.sin_addr);

    // Adaugam stdinul si socketul in multimea de citire
    FD_SET(STDIN_FILENO, &read_fds);
    FD_SET(sockfd, &read_fds);
    // Maximul e descriptorul de socket pentru ca,
    // descriptorul stdinului are valoarea 0( din pdf lab)
    fdmax = sockfd;

    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    while (1) {
        tmp_fds = read_fds;

        if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) {
          error("ERROR in select");
        }

        if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {
          // citesc de la tastatura
          memset(buffer, 0 , BUFLEN);
          fgets(buffer, BUFLEN - 1, stdin);

          // trimit mesaj la server
          n = send(sockfd, buffer, strlen(buffer), 0);
          if (n < 0)
               error("ERROR writing to socket");
        }

        if (FD_ISSET(sockfd, &tmp_fds)) {
            recv(sockfd, buffer, BUFLEN, 0);
            printf("%s", buffer);
        }
    }

    return 0;
}
