#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define port 8080
#define MAX_BUFFER_SIZE 1024


int main(int argc, char* argv[]){
	int sockfd, newfd;
	char buffer[MAX_BUFFER_SIZE];
	struct sockaddr_in server_addr;
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	connect(sockfd,(struct sockaddr*)(&server_addr), sizeof(server_addr));
	while(1){
		read(sockfd, buffer, sizeof(buffer));
		write(1,buffer,sizeof(buffer));
		printf("write your message.\n");
		read(0,buffer,sizeof(buffer));
		write(sockfd,buffer,sizeof(buffer));

	}

	return 0;
}
