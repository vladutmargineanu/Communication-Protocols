#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"

void send_icmp(int sock, struct sockaddr_in *whereto,
	      struct icmphdr *header, struct icmp_response *response)
{
	struct timespec start, end;

	clock_gettime(CLOCK_MONOTONIC, &start);
	int res = sendto(sock, header, sizeof(*header), 0, (struct sockaddr *)whereto, sizeof(*whereto));
	DIE(res < 0, "sendto");

	struct icmphdr rheader;
	struct sockaddr_in raddr;
	char buffer[256];
	socklen_t slength;
	res = recv(sock, buffer, sizeof(buffer), 0);
	DIE(res < 0, "recv");

	clock_gettime(CLOCK_MONOTONIC, &end);

	// XXX should check ihl?
	memcpy(&response->header, buffer + 20, 8);

	response->timeElapsed = (float)(end.tv_sec - start.tv_sec) +
				(end.tv_nsec - start.tv_nsec) / 1e9f;
}

int ping(int sock, struct sockaddr_in *whereto, int count)
{
	int res;

	struct icmphdr header;

	// TODO build and send packets using "send_icmp"
}

int main(int argc, char **argv)
{
	int res;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <count> <ip>\n", argv[0]);
		exit(1);
	}

	char *ip_addr, *hostname;
	struct sockaddr_in whereto;

	ip_addr = dns_lookup(argv[2], &whereto);
	DIE(ip_addr == NULL, "dns_lookup");

	hostname = reverse_dns_lookup(ip_addr);
	DIE(hostname == NULL, "reverse_dns_lookup");

	printf("PING %s (%s) ...\n", argv[2], ip_addr);

	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	DIE(sock < 0, "socket");

	int count = atoi(argv[1]);
	ping(sock, &whereto, count);

	return 0;
}
