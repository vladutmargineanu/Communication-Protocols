/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	client mini-server de backup fisiere
*/

/*/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	client mini-server de backup fisiere
*/

#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "helpers.h"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s ip_server port_server file\n", file);
	exit(0);
}

/*
*	Utilizare: ./client ip_server port_server nume_fisier_trimis
*/

int main(int argc, char **argv)
{
	if (argc != 4)
		usage(argv[0]);

	int fd;
	struct sockaddr_in to_station, my_sockaddr, from_station;
	char buffer[BUFLEN];

	/*Deschidere socket*/

	int socketfd;
	socketfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	/*Setare struct sockaddr_in pentru a asculta pe portul respectiv */

	my_sockaddr.sin_family = AF_INET;
	my_sockaddr.sin_port = htons(atoi(argv[1]));
	my_sockaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //foloseste inet_aton

	/* Legare proprietati de socket */

	from_station.sin_family = AF_INET;
	from_station.sin_port = 0;
	from_station.sin_addr.s_addr = htonl(INADDR_ANY);

	int noBytesWrited;
	int bindel;
	bindel = bind(socketfd, &(my_sockaddr), sizeof(struct sockaddr));

	/* Deschidere fisier pentru citire */

	DIE((fd = open(argv[3], O_RDONLY)) == -1, "open file");

	int sem;
	sem = 1;

	/*
	*  cat_timp  mai_pot_citi
	*		citeste din socket
	*		pune in fisier
	*/

	int numberOfbytesRecived;
	int len;
	printf("a primit mesaj\n");
	while (sem)
	{
		printf("a primit mesaj\n");
		numberOfbytesRecived = recvfrom(socketfd, buffer, BUFLEN, 0, &from_station, (socklen_t *)&len);
		noBytesWrited = write(fd, buffer, numberOfbytesRecived);
		printf("a primit mesaj\n");
		if (noBytesWrited < 0)
		{
			perror("Eroare la citire");
		}
		if (numberOfbytesRecived < BUFLEN)
		{
			sem = 0;
		}
		printf("a primit mesaj\n");
	}

	/*Inchidere socket*/

	close(socketfd);
	/*Inchidere fisier*/

	close(fd);
	return 0;
}

