#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define port 8080
#define MAX_BUFFER_SIZE 1024
#define BACKLOG 1024

int sockfd, newfd;

void *sendt(void *arg)
{
	char buffer[MAX_BUFFER_SIZE];
	while (1)
	{
		memset(buffer, 0, MAX_BUFFER_SIZE);
		// printf("write now:");
		fgets(buffer, MAX_BUFFER_SIZE, stdin);
		if (strncmp(buffer, "exit", 4) == 0)
		{
			printf("exiting....\n");
			exit(0);
		}
		// printf("%s",buffer);
		write(newfd, buffer, strlen(buffer));
	}
}

void *recvt(void *arg)
{
	char buffer[MAX_BUFFER_SIZE];
	int ret = 0;
	while (1)
	{
		ret = 0;
		memset(buffer, 0, MAX_BUFFER_SIZE);
		ret = read(newfd, buffer, MAX_BUFFER_SIZE - 1);
		if (ret < 0)
		{
			printf("cant get message from client .\n");
			return NULL;
		}

		if (ret == 0)
		{
			printf("client disconnected .\n");
			return NULL;
		}

		if (ret < MAX_BUFFER_SIZE)
		{
			buffer[ret] = '\0';
		}

		buffer[ret] = '\0';
		printf("client:%s", buffer);
	}

	return NULL;
}

int main(int argc, char *argv[])
{

	pthread_t ts, tr;
	socklen_t len;
	struct sockaddr_in addr, clientaddr;

	// socket creation
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("binding failed.\n");
		return -1;
	}
	printf("socket created and its number is : %d.\n", sockfd);

	// binding process
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((bind(sockfd, (struct sockaddr *)(&addr), sizeof(addr))) < 0)
	{
		close(sockfd);
		perror("binding to the port was unsuccessful.\n");
		return -1;
	}

	printf("binded to the port: %d...\n", port);

	// make the socket passive by making it listen
	if (listen(sockfd, BACKLOG) < 0)
	{
		perror("problem while listening.\n");
		close(sockfd);
		return -1;
	}

	printf("listening to the port %d....\n ", port);

	// accepting part

	memset(&clientaddr, 0, sizeof(clientaddr));
	len = sizeof(clientaddr);

	//	while(1){
	if ((newfd = accept(sockfd, (struct sockaddr *)(&clientaddr), &len)) < 0)
	{
		perror("problem in creating newfd.\n");
	}

	printf("connected to the client.\n");
	pthread_create(&ts, NULL, sendt, NULL);
	pthread_create(&tr, NULL, recvt, NULL);

	pthread_join(ts, NULL);
	pthread_join(tr, NULL);
	//	}

	close(sockfd);
	close(newfd);

	return 0;
}
