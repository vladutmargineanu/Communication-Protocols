#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    int i;
    char *message;
    char *response;
    int sockfd;

    
    /*
    *   Ex 0: Get cs.curs.pub.ro
    *   
    *   Pas 1: Se deschide conexiunea (open_connection)
    *   Pas 2: Se creaza mesajul de request (compute_get_request)
    *   Pas 3: Se trimite la server mesajul (send_to_server)
    *   Pas 4: Se primeste raspuns de la server (receive_from_server)
    *   Pas 5: Se inchide conexiunea cu serverul (close_connection)
    */

    //TODO EX 0

   /* char* ipHost = argv[1];
    sockfd = open_connection(ipHost , 80, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(ipHost, "/", NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", message);
    printf("%s", response);
    close_connection(sockfd);
*/
    /*
        Ex 1: Get videos
    */
    
    // TODO EX 1
    char* host = argv[1];
    sockfd = open_connection(host , 8081, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(host, "/videos", NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", response);
    close_connection(sockfd);


    /*
        EX 2.1: Add video
    */
    char *form_data = "id=15&name=vinePastele";
    sockfd = open_connection(host, 8081, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(host, "/videos", form_data);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", response);
    close_connection(sockfd);
    // TODO Ex 2.1

     /*
        Ex 2.2 Verificam noua colectie de videoclipuri
    */
    
    sockfd = open_connection(host , 8081, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(host, "/videos", NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", response);
    close_connection(sockfd);
    // TODO Ex 2.2
    
    /*
        Ex 3 Autentificare
    */

    // TODO Ex 3
    char* data ="username=admin&password=p@ss";
    sockfd = open_connection(host , 8081, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(host, "/weather/login", data);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", response);
    close_connection(sockfd);

    char* cookie = "isLogged=true";
    sockfd = open_connection(host , 8081, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(host, "/weather/key", cookie);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s", response);
    close_connection(sockfd);

    
    free(message);
    return 0;
}