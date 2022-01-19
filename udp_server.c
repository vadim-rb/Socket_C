#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include  <signal.h>

#define DEFAULT_PORT 9999
#define MAXBUF 1024
#define PANIC(msg)  { perror(msg); exit(-1); }

int sockfd;
int clientfd;

void SIGINThandler(int x){
    printf("\nHit Ctrl-C...exit\n");
    close(clientfd);
    close(sockfd);
    exit(EXIT_SUCCESS);
    
}


int main(int argc, char *argv[])
{
    int port = DEFAULT_PORT;
    struct sockaddr_in addr;
    char buffer[MAXBUF];

    if (argc==3 && (strcmp(argv[1], "-p") == 0))
    {
        port = atoi(argv[2]);//bad
	if (port == 0 || port<1024 || port>65535)
	{
		PANIC("Incorrect port(must be in range 1024-65535)\n");
	}
    }

    printf("Starting UDP server on port %d\n",port);

    //---Create socket---//
    if ((sockfd = socket(AF_INET, SOCK_DGRAM , 0)) == -1)
	{
		PANIC("Error while creating a socket: ");
	}
//---Initialize address/port structure---//
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

	/*---Assign a port number to the socket---*/
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1 )
	{
		PANIC("Error while bind a socket: ");
	}
    while (1)
	{	int bytes, addr_len=sizeof(addr);
		bzero(buffer, MAXBUF);

		bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);
		printf("msg from %s:%d (%d bytes) : %s\n", inet_ntoa(addr.sin_addr),
						ntohs(addr.sin_port), bytes,buffer);

		sendto(sockfd, buffer, bytes, 0, (struct sockaddr*)&addr, sizeof(addr));
	}
	close(sockfd);

}