//client.bin -h 127.0.0.1 -p 9001 -d hello
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if (argc!=7){
	printf("Incorrect number of arguments\n");
	exit(EXIT_FAILURE);
	}
	if (strcmp(argv[1], "-h") != 0 
	 || strcmp(argv[3], "-p") != 0
	 || strcmp(argv[5], "-d") != 0){
	printf("Incorrect keys\n");
	printf("%s %s %s \n",argv[1],argv[3],argv[5]);
	}
	char *ip = argv[2];
	char *port = argv[4];
	char *data = argv[6];
	printf("Values is %s %s %s \n",argv[2],argv[4],argv[6]);
	char message[strlen(data)+1];
	strcpy(message, data);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	int result_inet_pton = inet_pton(AF_INET, ip, &(addr.sin_addr.s_addr));
	if (result_inet_pton != 1){
	printf("Incorrect ip addr\n");
	exit(EXIT_FAILURE);
	}
	int port_int = atoi(port);//bad
	if (port_int == 0 || port_int<1024 || port_int>65535){
	printf("Incorrect port(must be in range 1024-65535)\n");
	printf("Port is %d\n",port_int);
	exit(EXIT_FAILURE);
	}
	addr.sin_port = htons(port_int);
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		perror("Error while creating a socket: ");
		exit(EXIT_FAILURE);
	}
	int connect_result;
	connect_result = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	if(connect_result == -1){
        perror("Error while connect: ");
        exit(EXIT_FAILURE);
        }
	char buf[1024];
	memset(buf,0,sizeof(buf));
	
        send(sock, message, sizeof(message), 0);
        recv(sock, buf, sizeof(buf), 0);
        printf("Recived data : %s\n",buf);
        close(sock);
	return 0;
}