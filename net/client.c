#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_BUFF_SIZE 256

int main(int argv,char *args[]){

	int fd_socket;
    struct sockaddr_in sockaddr;
    char buf[MAX_BUFF_SIZE];

    bzero(buf,MAX_BUFF_SIZE);


    fd_socket = socket(AF_INET,SOCK_STREAM,0);

    bzero(&sockaddr,sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(8888);
    sockaddr.sin_addr.s_addr = inet_addr(args[1]);


    connect(fd_socket,(struct sockaddr*)&sockaddr,sizeof(sockaddr));

    read(fd_socket,buf,MAX_BUFF_SIZE);
    printf("%s",buf);

    bzero(buf,MAX_BUFF_SIZE);
    printf("input:");
    scanf("%s",buf);

    write(fd_socket,buf,MAX_BUFF_SIZE);

    close(fd_socket);
	return 1;
}



