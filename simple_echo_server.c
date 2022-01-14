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

#define PORT 9005
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

int main()
{
    
    char buf[1024];
    int bytes_read;
    int server_socket;
    int bind_status;
    int listen_status;
    struct sockaddr_in addr; 
    struct  sockaddr_in client_address;
    char client_ip[INET_ADDRSTRLEN];
    int client_address_len;
/*
	Создание сокета

int socket(int domain, int type, int protocol);

Константа AF_INET соответствует Internet-домену.
SOCK_STREAM используется протокол TCP
Протокол однозначно определяется по домену и типу сокета.
socket = 0,соответствует протоколу по умолчанию. 
Возвращает значение:
При успехе, a file descriptor for the new socket is returned.  
При ошибке, -1 is returned, and errno is set appropriately.
*/
    printf("Starting server...\n");
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1)
    {
        perror("Error while creating a socket: ");
        exit(EXIT_FAILURE);
    }
/*
	Именование сокета

int bind(int sockfd, struct sockaddr *addr, int addrlen);

Сокет необходимо связать с адресом в выбранном домене
(эту процедуру называют именованием сокета).

sockfd это дескриптор сокета, уже получен в server_socket
addr, содержит указатель на структуру с адресом,
Создаем структуру с адресом для функции именования сокета bind
Будем использовать вместо sockaddr одну из альтернативных структур 
вида sockaddr_XX (XX - суффикс, обозначающий домен: "un" - Unix,
 "in" - Internet и т. д.).
При передаче в функцию bind указатель на эту структуру приводится
к указателю на sockaddr.
addrlen - длинa структуры addr
*/
    
    addr.sin_family = AF_INET; // Семейство адресов
//При указании IP-адреса и номера порта необходимо преобразовать число из порядка хоста в сетевой.
    addr.sin_port = htons(PORT); // Номер порта
//INADDR_ANY все интерфейсы
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // IP-адрес

/*
int bind(int sockfd, struct sockaddr *addr, int addrlen);
RETURN VALUE
On  success,  zero is returned.
On error, -1 is returned, and errno is set appropriately.
*/
    
    bind_status = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if(bind_status == -1)
    {
        perror("Error while bind a socket: ");
        exit(EXIT_FAILURE);
    }
/*
int listen(int sockfd, int backlog);
ервый параметр - дескриптор сокета, а второй задаёт размер очереди запросов.
Cоздаётся очередь запросов на соединение. 
При этом сокет переводится в режим ожидания запросов со стороны клиентов.
RETURN VALUE
       On  success,  zero is returned.  On error, -1 is returned, and errno is
       set appropriately.
*/
    
    listen_status = listen(server_socket, BACKLOG);
    if(listen_status == -1)
    {
        perror("Error while listen: ");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, SIGINThandler);
    while(keep_socket)
    {
/*
	Прием данных
int accept(int sockfd, void *addr, int *addrlen);
Когда сервер готов обслужить очередной запрос, он использует функцию accept.
Функция accept создаёт для общения с клиентом новый сокет и возвращает 
его дескриптор. Параметр sockfd задаёт слушающий сокет. 
После вызова он остаётся в слушающем состоянии и может принимать другие 
соединения.В структуру, на которую ссылается addr, записывается адрес сокета клиента,
который установил соединение с сервером. В переменную, адресуемую 
указателем addrlen, изначально записывается размер структуры;
функция accept записывает туда длину, которая реально была использована.
Если вас не интересует адрес клиента, вы можете просто передать NULL в 
качестве второго и третьего параметров.

Обратите внимание, что полученный от accept новый сокет связан с тем же самым
адресом, что и слушающий сокет. Сначала это может показаться странным.
 Но дело в том, что адрес TCP-сокета не обязан быть уникальным в 
Internet-домене. Уникальными должны быть только соединения, 
для идентификации которых используются два адреса сокетов, между которыми 
происходит обмен данными.

RETURN VALUE         
       On success, these system calls return a file descriptor for the
       accepted socket (a nonnegative integer).  On error, -1 is
       returned, errno is set to indicate the error, and addrlen is left
       unchanged.
*/	
	client_address_len = sizeof(client_address);
        sock = accept(server_socket,(struct sockaddr *)&client_address, &client_address_len);
        if(sock == -1)
        {
            perror("Error while accept: ");
            exit(EXIT_FAILURE);
        }

	
	inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip));

	printf("Connected with address %s\n", client_ip);

	
        while(keep_recv)
        {
/*
Для чтения данных из сокета используется функция recv.
int recv(int sockfd, void *buf, int len, int flags);
на  принимает дескриптор сокета, указатель на буфер и набор флагов.
RETURN VALUE
       These  calls  return  the  number  of bytes received, or -1 if an error
       occurred.  In the event of an error,  errno  is  set  to  indicate  the
       error.

       When a stream socket peer has performed an orderly shutdown, the return
       value will be 0 (the traditional "end-of-file" return).

       Datagram sockets in  various  domains  (e.g.,  the  UNIX  and  Internet
       domains)  permit  zero-length  datagrams.   When  such  a  datagram  is
       received, the return value is 0.

       The value 0 may also be returned if the requested number  of  bytes  to
       receive from a stream socket was 0.

*/
	    printf("Waiting for data...\n");
	    memset(buf,0,sizeof(buf));
            bytes_read = recv(sock, buf, 1024, 0);
	    printf("bytes_read %d \n",bytes_read);
            if(bytes_read == -1 || bytes_read == 0) break;
	    if (strcmp (buf, STOP_WORLD)==0){
		printf("Recived stop world...exit\n");
		close(sock);
		exit(EXIT_SUCCESS);
	    }
	    printf("Recived data is : %s\n", buf);
            send(sock, buf, bytes_read, 0);
	//break;//debug

        }
    
        //break;//debug
    }
    close(sock);
    return 0;
}