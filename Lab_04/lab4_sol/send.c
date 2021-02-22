#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST 		"127.0.0.1"
#define PORT 		10000
#define FRAME_SIZE	1404  // dimensiunea ferestrei
#define BITS_NO		8  // byte
/* legatura de date == cilindru */

int main(int argc, char *argv[])
{
	msg t;
	int i, res;
	long bdp, window;
	
	printf("[SENDER] Sender starts.\n");	
	init(HOST, PORT);

	/* miliseconds for delay & megabits for speed */
	/* atol = Parses the C-string str interpreting its content as an integral number,
	which is returned as a value of type long int.*/
	/* cantitatea de informatie = bandwith (V) * delay (Tp) */
	bdp = atol(argv[1]) * 1000;
	printf("[SENDER] BDP = %ld b(bits).\n", bdp);
	printf("[SENDER] Gonna send %d frames.\n", COUNT);
	
	/* window = number of frames in the 'network', unacked */
	/* W (window) = numarul maxim de cadre neconfirmate la orice moment de timp */
	window = bdp / (FRAME_SIZE * BITS_NO); 
	printf("[SENDER] window = %ld frames\n", window);

	/* cleanup msg */
	memset(&t, 0, sizeof(msg));
	    /* RTT (Round Trip Time) = timpul scurs din momentul in care un cadru este
	    trimis pana in momentul in care este primita confirmarea. */
		/* RTT = T P => un singur cadru in legatura de date pe parcursul unui RTT */
		/* Fill the link = send window messages */
		for (i = 0; i < window; i++)
	{
		/* gonna send an empty msg */
		memset(t.payload, 0, MSGSIZE);
		t.len = MSGSIZE;
		
		/* send msg */
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	/* From now on, ack clocking, i.e., a new ack will inform 
	   us about the space released in the link (legatura de date) */
	for (i = 0; i < COUNT - window; i++) {
		/* wait for ACK */
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		} else {
			printf("[SENDER] Received ACK %s\n", t.payload);
		}

		/* gonna send an empty msg */
		memset(t.payload, 0, MSGSIZE);
		t.len = MSGSIZE;
		
		/* send msg */
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	/* so far: COUNT x send
		   (COUNT - window) x ack
	   So we need to wait for another 'window' acks */
	for (i = 0; i < window; i++) {
		/* recv ACK */
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}  else {
			printf("[SENDER] Received ACK %s\n", t.payload);
		}
	}


	printf("[SENDER] Job done, all sent.\n");
		
	return 0;
}
 /* Teste din cartea Dandamudi */ 