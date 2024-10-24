#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define port 8081
#define BACKLOG 10
#define MAX_BUFFER_SIZE 1024


int main(int argc , char *argv[]){
	int sockfd,newfd,ret;
	socklen_t lent;
	struct sockaddr_in addr, claddr;
	char buffer[MAX_BUFFER_SIZE];
	

	//socket creation
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))< 0){
		printf("socket creation failed.\n");
		return -1;	
	}

	printf("sockfd = %d\n",sockfd);

	memset(&addr,0,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	lent = sizeof(struct sockaddr);

	//binding with the address 
	if(bind(sockfd,(struct sockaddr*)(&addr),lent) <  0){
		printf("Binding failed.\n");
		return -1;			
	}
	printf("listening at port : %d", ntohs(addr.sin_port));

	// listen for any connection requests
	if(listen(sockfd,BACKLOG)> 0){
		printf("listening failed");
		return -1;
	}


	//accept the requests.
	int len = sizeof(claddr);
	if((newfd = accept(sockfd,(struct sockaddr*)(&claddr),&len)) == -1){
		printf("connection accept failed.\n");
		return -1;
	}

	printf("connection accepted.\n");
	
	ret = read(newfd,buffer,MAX_BUFFER_SIZE);
	if(ret <= 0){
		printf("read failed.\n");
		return -1;
	}
	
	while(1){
			
		printf("message from client %s",buffer);
		read(0,buffer,MAX_BUFFER_SIZE);
		write(newfd,buffer,MAX_BUFFER_SIZE);
	}




	return 0;
}
