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

#define DEFAULT_PORT 9005
#define BACKLOG 1
#define STOP_WORLD "stop"

int keep_socket = 1;
int keep_recv = 1;
int sock;

void SIGINThandler(int x){
    printf("\nHit Ctrl-C...exit\n");
    close(sock);
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
    int port_int = DEFAULT_PORT;
    if ((strcmp(argv[1], "-p") == 0) && argc==3)
    {
        port_int = atoi(argv[2]);//bad
	if (port_int == 0 || port_int<1024 || port_int>65535){
	printf("Incorrect port(must be in range 1024-65535)\n");
	exit(EXIT_FAILURE);
	}
    }
    int bytes_read;
    int server_socket;
    int bind_status;
    int listen_status;
    struct sockaddr_in addr; 
    struct  sockaddr_in client_address;
    char client_ip[INET_ADDRSTRLEN];
    int client_address_len;
    printf("Starting server...\n");
    printf("Binding port is %d\n",port_int);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1)
    {
        perror("Error while creating a socket: ");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(port_int); 
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 
 
    bind_status = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if(bind_status == -1)
    {
        perror("Error while bind a socket: ");
        exit(EXIT_FAILURE);
    }    
    listen_status = listen(server_socket, BACKLOG);
    if(listen_status == -1)
    {
        perror("Error while listen: ");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, SIGINThandler);



    while(keep_socket)
    {	
	client_address_len = sizeof(client_address);
        sock = accept(server_socket,(struct sockaddr *)&client_address, &client_address_len);
        if(sock == -1)
        {
            perror("Error while accept: ");
            exit(EXIT_FAILURE);
        }
	inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip));
	printf("Connected with address %s\n", client_ip);
	pid_t pid = fork();
	switch(pid)
	{
	  case -1: 
		perror("Error while fork: ");
	        exit(EXIT_FAILURE);//при вызове fork() возникла ошибка
	  case 0 : // код потомка
		close(server_socket);
		process_request(sock);
		close(sock);
		exit(0);
	  default : //код родительского процесса
		close(sock);
	}
    }

    return 0;
}