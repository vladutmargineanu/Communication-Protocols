/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * client.c
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFLEN 1500

int main(int argc, char *argv[]) {
    int sockfd = 0;
    char server_reply[BUFLEN], message[BUFLEN];
    struct sockaddr_in serv_addr;

    if (argc != 3) {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // deschidere socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Could not create socket\n");
        return -1;
    }
    printf("Socket created\n");
    // completare informatii despre adresa serverului
    inet_aton(argv[1], &serv_addr.sin_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    printf("sin_addr: %s\n", inet_ntoa(serv_addr.sin_addr));
    printf("sin_port: %d\n", serv_addr.sin_port);

    // conectare la server
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed. Error\n");
        return 1;
    }
    printf("Connected\n");
    /* citire de la tastatura, trimitere de
     cereri catre server, asteptare raspuns */
        while (1) {
        printf("Enter message: ");

        memset(message, 0, BUFLEN);
        scanf("%s", message);
        // Send some data
        if (send(sockfd, message, strlen(message), 0) < 0) {
            printf("Send failed\n");
            return 1;
        }
        memset(server_reply, 0, BUFLEN);
        printf("Sended message");
        // Receive a reply from the server
        if (recv(sockfd, server_reply, BUFLEN, 0) < 0) {
            printf("recv failed\n");
            break;
        }
        printf("Server reply: %s\n", server_reply);

        if (strncmp(message, "quit", 4) == 0) {
            printf("ok");
            break;
        }
    }
    // inchidere socket
    close(sockfd);
    return 0;
}
