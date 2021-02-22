#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define HTTP_PORT 80
#define MAXLEN 500



/**
 * Citeste maxim maxlen octeti din socket-ul sockfd. Intoarce
 * numarul de octeti cititi.
 */
ssize_t Readline(int sockd, void * vptr, size_t maxlen) {
    ssize_t n, rc;
    char c, * buffer;

    buffer = vptr;

    for (n = 1; n < maxlen; n++) {
        if ((rc = read(sockd, & c, 1)) == 1) { * buffer++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1)
                return 0;
            else
                break;
        } else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }

    * buffer = 0;
    return n;
}

/**
 * Trimite o comanda HTTP si asteapta raspuns de la server.
 * Comanda trebuie sa fie in buffer-ul sendbuf.
 * Sirul expected contine inceputul raspunsului pe care
 * trebuie sa-l trimita serverul in caz de succes (de ex. codul
 * 200). Daca raspunsul semnaleaza o eroare se iese din program.
 */
void send_command(int sockfd, char sendbuf[], char * expected) {
    char recvbuf[MAXLEN];
    int nbytes;


    //TODO: trimitere comanda
    printf("Trimit: %s", sendbuf);
    write(sockfd, sendbuf, strlen(sendbuf));

    nbytes = Readline(sockfd, recvbuf, MAXLEN - 1);
    recvbuf[nbytes] = 0;
    // printf("%s\n", recvbuf);
    if (strstr(recvbuf, expected) != recvbuf) {
        printf("Eroare de la server!\n");
        exit(-1);
    } else {
        printf("%s\n\n", recvbuf);
    }
}

int main(int argc, char * * argv) {
    int sockfd;
    int port = HTTP_PORT;
    struct sockaddr_in servaddr;
    char server_ip[10];
    char sendbuf[MAXLEN];
    char recvbuf[MAXLEN];

    if (argc != 2) {
        printf("Utilizare: ./send_msg adresa_server");
        exit(-1);
    }
    strcpy(server_ip, argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Eroare la  creare socket.\n");
        exit(-1);
    }

    /* formarea adresei serverului */
    memset( & servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_aton(server_ip, & servaddr.sin_addr) <= 0) {
        printf("Adresa IP invalida.\n");
        exit(-1);
    }

    /*  conectare la server  */
    if (connect(sockfd, (struct sockaddr * ) & servaddr, sizeof(servaddr)) < 0) {
        printf("Eroare la conectare\n");
        exit(-1);
    }


    //TODO: trimitere comanda HTTP
    sprintf(sendbuf, "GET / HTTP/1.1\n\n");
    send_command(sockfd, sendbuf, "HTTP/1.1 200 OK");

	//TODO: citire continut si afisare
    while (Readline(sockfd, recvbuf, MAXLEN - 1)) {
        printf("%s",recvbuf);
    }

    close(sockfd);
	return 0;
}
