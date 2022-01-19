#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE		1024
#define PANIC(msg)  { perror(msg); exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{	char buffer[BUFSIZE];
	struct sockaddr_in addr;
	char *ip;
	char *port;
	char *data;
	int port_int;
	int sock;
	int addr_size;
        int sd;

        printf("Client UDP /(*)(*)/\n");
	if (argc!=7){
	PANIC("Incorrect number of arguments");
	}

	if (strcmp(argv[1], "-h") != 0 
	 || strcmp(argv[3], "-p") != 0
	 || strcmp(argv[5], "-d") != 0){
	printf("Incorrect keys\n");
	printf("%s %s %s \n",argv[1],argv[3],argv[5]);
	}
	ip = argv[2];
	port = argv[4];
	data = argv[6];
	printf("Values is %s %s %s \n",argv[2],argv[4],argv[6]);

	char message[strlen(data)+1];
	strcpy(message, data);
	addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, ip, &(addr.sin_addr.s_addr)) != 1){
	PANIC("Incorrect ip addr\n");
	}
	port_int = atoi(port);//bad
	if (port_int == 0 || port_int<1024 || port_int>65535){
	printf("Incorrect port(must be in range 1024-65535)\nPort is %d\n",port_int);
	exit(EXIT_FAILURE);
	}
	addr.sin_port = htons(port_int);
	

    if ((sd = socket(AF_INET, SOCK_DGRAM , 0)) == -1){
		PANIC("Error while creating a socket: ");
	}

	printf("sendto\n");
	sendto(sd, message, strlen(message), 0, (struct sockaddr*)&addr, sizeof(addr));
	bzero(buffer, BUFSIZE);
	addr_size = sizeof(addr);
	printf("recvfrom\n");
	if ( recvfrom(sd, buffer, BUFSIZE, 0, (struct sockaddr*)&addr, &addr_size) < 0 )
		perror("server reply");
	else
		printf("Reply: %s:%d \"%s\"\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buffer);
	close(sd);
	return 0;
}
