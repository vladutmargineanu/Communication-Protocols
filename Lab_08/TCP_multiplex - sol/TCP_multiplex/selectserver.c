#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS	5
#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[BUFLEN], mini_buffer[4];
     struct sockaddr_in serv_addr, cli_addr;
     int n, update_new_client = 0;

		int i, j, k;
		int clients[MAX_CLIENTS];

     fd_set read_fds;    // multimea de citire folosita in select()
    fd_set tmp_fds;      // multimea de citire auxiliara( ptr select care modifica lista primita)
    int fdmax;          // valoarea maxima a file descriptorului din multimea read_fds

    if (argc < 2) {
       fprintf(stderr,"Usage : %s port\n", argv[0]);
       exit(1);
    }

    // lista de clienti
    for (i = 0; i < MAX_CLIENTS; i++) {
       clients[i] = 0;
    }

     // Ne asiguram ca multimile de citire sunt initial vide
     FD_ZERO(&read_fds);
     FD_ZERO(&tmp_fds);
		 FD_SET(STDIN_FILENO, &read_fds);
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     
     portno = atoi(argv[1]);

     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd, MAX_CLIENTS);

     //adaugam noul file descriptor
	   //(socketul pe care se asculta conexiuni) in multimea read_fds
     FD_SET(sockfd, &read_fds);
     fdmax = sockfd;

     // main loop
	while (1) {
		tmp_fds = read_fds; 

		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
			error("ERROR in select");
	
		for(i = 0; i <= fdmax; i++) {

			if (FD_ISSET(i, &tmp_fds)) {
			
				if (i == sockfd) {
					// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
					// actiunea serverului: accept()
					clilen = sizeof(cli_addr);

					if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
						error("ERROR in accept");
					} 

					else {
						//adaug noul socket intors de accept() la multimea descriptorilor de citire
						FD_SET(newsockfd, &read_fds);

						if (newsockfd > fdmax) { 
							fdmax = newsockfd;
						}

						for (j = 0; j < MAX_CLIENTS; j++) {
              if (clients[j] == 0) {

                  clients[j] = newsockfd;
                  break;

              }
            }

						if (fdmax > sockfd) {
							for (j = sockfd + 1; j <= fdmax; j++) {

              	if (j != newsockfd) {

                	sprintf(buffer, "Server: New client available: %d\n", newsockfd);
                	send(j, buffer, strlen(buffer), 0);

								} else {
									update_new_client = 0;

                  for (k = 0; k < MAX_CLIENTS; k++) {
                    if (clients[k] != 0) {
                        update_new_client = 1;
                        break;
                    }
                  }

									if (update_new_client) {
                    sprintf(buffer, "Server: Available clients:");

                    for (k = 0; k < MAX_CLIENTS; k++) {

                        if (clients[k] != 0) {
                            strcat(buffer, " ");
                            sprintf(mini_buffer, "%d", clients[k]);
                            strcat(buffer, mini_buffer);
                        }
                    }

                    strcat(buffer, "\n");
                    send(newsockfd, buffer, strlen(buffer), 0);

		              } else {
		                  sprintf(buffer, "Server: No other clients \n");
		                  send(newsockfd, buffer, strlen(buffer), 0);
		              }
								}
							}
						}
					}

					printf("Noua conexiune de la %s, port %d, socket_client %d\n ", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
				}
					
				else {
					// am primit date pe unul din socketii cu care vorbesc cu clientii
					//actiunea serverului: recv()
					memset(buffer, 0, BUFLEN);
					if(i != 0) {
						if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {

							if (n == 0) {
								//conexiunea s-a inchis
								printf("selectserver: socket %d hung up\n", i);

								for (j = 0; j < MAX_CLIENTS; j++) {
		            	if (clients[j] == i) {
		                    clients[j] = 0;
		                    break;
		              }
		            }

		            if (fdmax > sockfd) {
				          for (j = sockfd + 1; j <= fdmax; j++) {
			              if (j != i) {
			                  sprintf(buffer, "Server: Client %d left.\n", i);
			                  send(j, buffer, strlen(buffer), 0);
			              }
				          }
		            }

							} else {
								error("ERROR in recv");
							}

							close(i); 
							FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care 
						} 
					
						else { //recv intoarce >0
							printf ("Am primit de la clientul de pe socketul %d, mesajul: %s\n", i, buffer);
	
							if (buffer[0] - '0' <= 10) {

		            send(buffer[0] - '0', buffer, strlen(buffer), 0);

		          } else {
									for (j = 1; j <= fdmax; j++) {
						        if ((j != sockfd) && (j != i)) {

						            send(j, buffer, strlen(buffer), 0);

						        }
									}
							}
						}

					} else {
						memset(buffer, 0, BUFLEN);
        		scanf("%s", buffer);					

						if (strstr(buffer, "exit")) {
							printf ("Closing server\n");
							return;
						}
					}


				} 
			}
		}
     }


     close(sockfd);
   
     return 0; 
}

