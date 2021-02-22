#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* stat*/
#include <fcntl.h>
#include <sys/types.h> /* stat*/
#include <sys/stat.h>  /* stat*/

#include "lib.h"	

#define HOST 	"127.0.0.1"
#define PORT 	10000

int main(int argc, char *argv[])
{
	msg t;
	my_pkt p;
	int fd, count, filesize;
	struct stat f_status;
	char buffer[MSGSIZE];
	
	init(HOST, PORT);
	printf("[SENDER] Server rocks.\n");
	printf("[SENDER] File to send: %s\n", argv[1]);
	/* argv[1] este numele fisierului transmis ca parametru */
	// fd retine descriptorul fisierului deschis
	fd = open(argv[1], O_RDONLY);
	/* int fstat(int fd, struct stat *buf); */
	// f_status contine toate informatiile din structura fstat
	fstat(fd, &f_status);
	/* se face cast */
	filesize = (int) f_status.st_size;
	printf("[SENDER] File size: %d\n", filesize);

	/* Len in message 
		= size(msg payload)
		= amount of data in the my_pkt structure
		= size(type) + data in (pkt payload)
		= sizeof(int) + number of used bytes in (pkt payload)
	*/

	/* Gonna send filename - TYPE 1 message */

	/* Setam cu 0 primii sizeof(t.payload) byte
	payload = incarcatura utila */
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	// my_pkt p;
	p.type = TYPE1;	//TYPE1 trimite numele fisier (are valoarea 1)
	memcpy(p.payload, argv[1], strlen(argv[1])); //pune in my_pkt.payload numele fisierului de trimis
	t.len = sizeof(int) + strlen(argv[1]);//t.len==lungimea pachetului p (my_pkt)
										  //int (type) + strlen(nume) (numele fisierului)
	memcpy(t.payload, &p, t.len); 		  //pune in t.payload pe p
	send_message(&t);  // trimitem msg t
	printf("[SENDER] Filename sent.\n");

	/* Wait for filename ACK */	
	if (recv_message(&t) < 0) {
		perror("[SENDER] Receive error");
		return -1;
	}
	/* Facem cast la my_pkt pentru t.payload
	t.payload = contine: int type; char payload[PKTSIZE]; din structura my_pkt */
	
	// VERIFICAM confirmarea din recv.c (p.type este TYPE4)
	p = *((my_pkt *) t.payload);
	if (p.type != TYPE4) {
		perror("[SENDER] Receive error");
		return -1;
	}
	printf("[SENDER] Got reply with payload: %s\n", p.payload);
	
	/* Gonna send filesize - TYPE 2 message */
	memset(t.payload, 0, sizeof(t.payload));
	memset(p.payload, 0, sizeof(p.payload));
	
	p.type = TYPE2;
	memcpy(p.payload, &filesize, sizeof(int)); //adauga lungime fisier
	t.len = sizeof(int) * 2;	//2 inturi :type+filesize
	// in t.payload copiem toata toata structura p
	// se copiaza bit cu bit structura p in t.payload
	memcpy(t.payload, &p, t.len);
	send_message(&t);
	printf("[SENDER] Filesize sent.\n");
	
	/* Wait for filesize ACK */	
	if (recv_message(&t) < 0) {
		perror("[SENDER] Receive error");
		return -1;
	}
	// VERIFICAM confirmarea din recv.c (p.type este TYPE4)
	p = *((my_pkt *) t.payload);
	if (p.type != TYPE4) {
		perror("[SENDER] Receive error");
		return -1;
	}
	printf("[SENDER] Got reply with payload: %s\n", p.payload);

	/* Send file contents - TYPE 3 messages */
	printf("[SERVER] File transfer begins.\n");
	// Cat timp citim din fisier
	while((count = read(fd, buffer, MSGSIZE - sizeof(int))) > 0) {
		//face MSGSIZE - sizeof(int) cdeoarece atat e sizeof(my_pkt) (cu 4 byte mai putin = un int)
		memset(t.payload, 0, sizeof(t.payload));
		memset(p.payload, 0, sizeof(p.payload));
		
		p.type = TYPE3;
		memcpy(p.payload, buffer, count);
		//t.len==lungimea pachetului p (my_pkt)
		//int (type) + strlen(nume) (numele fisierului)
		t.len = sizeof(int) + count; //type + nr bytes cititi
		memcpy(t.payload, &p, t.len);
		send_message(&t);
		/* Wait for filesize ACK */
		if (recv_message(&t) < 0) {
			perror("[SENDER] Receive error");
			return -1;
		}
		// verificare ACK_T3 (trimitere continut)
		p = *((my_pkt *) t.payload);
		// TYPE4 este pentru confirmarea ack urilor
		if (p.type != TYPE4) {
			perror("[SENDER] Receive error");
			return -1;
		}
	}
	
	printf("[SERVER] File transfer has ended.\n");	
	close(fd);
	
	return 0;
}
/* Trimitem pe rand:
1. Numele fisierului
2. Lungimea fisierului
3. Continutul fisierlui
TYPE4 este pentru confirmarea ack urilor, daca s-a primit mesajul,
din recv.c trimitem confirmarea.

TYPE1 ESTE PENTRU numele fisierului
TYPE2 ESTE PENTRU lungimea fisierlui
TYPE3 ESTE PENTRU mesajul (continutul) fisierului

Fisierele recv.c si send.c merg in paralel. Codul trebuie 
parcurs in paralel cu fiecare actiune care are loc in timpul respectiv.
Algoritm:
1. Se deschide fisierul. Trimitem numele fisierlui din send.c
2. In recv.c se asteapta mesajul trimis din send.c si
se verifica daca este de TYPE1. Apoi trimitem ACK de TYPE4 lui send.c pentru
confirmare.
3. In send.c verificam daca am primit ack ul de TYPE4 si continuam cu aceeasi pasi (1 2 3)..

*/ 
