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
#define BACKLOG 10
#define PANIC(msg)  { perror(msg); exit(-1); }

int sockfd;
int clientfd;

void SIGINThandler(int x){
    printf("\nHit Ctrl-C...exit\n");
    close(clientfd);
    close(sockfd);
    exit(EXIT_SUCCESS);
    
}

void process_request(int sock){
    char buf[1024];
    memset(buf,0,sizeof(buf));
    printf("Waiting for data...process %d\n",getpid());
    int bytes_read = recv(sock, buf, 1024, 0);
    if(bytes_read == -1 || bytes_read == 0) return ;
    printf("Recived data is : %s\n", buf);
    send(sock, buf, bytes_read, 0);
    return ;
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

    printf("Starting server on port %d\n",port);

    //---Create socket---//
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
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

	/*---Make it a "listening socket"---*/
    if (listen(sockfd, BACKLOG) == -1 )
	{
		PANIC("Error while listen: ");
	}

    signal(SIGINT, SIGINThandler);
    while (1)
	{	
		struct sockaddr_in client_addr;
		int addrlen=sizeof(client_addr);
		printf("Waiting for connect...\n");

		/*---accept a connection (creating a data pipe)---*/
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
		printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		pid_t pid = fork();
		switch(pid)
		{
		  case -1: 
			perror("Error while fork: ");
			exit(EXIT_FAILURE);//при вызове fork() возникла ошибка
		  case 0 : // код потомка
			close(sockfd);
			process_request(clientfd);
			close(clientfd);
			exit(0);
		  default : //код родительского процесса
			close(clientfd);
		}
	}
}