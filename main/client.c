#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 1024

int sockfd;

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
		write(sockfd, buffer, strlen(buffer));
	}
}

void *recvt(void *arg)
{
	char buffer[MAX_BUFFER_SIZE];
	int ret = 0;
	while (1)
	{
		memset(buffer, 0, MAX_BUFFER_SIZE);
		ret = read(sockfd, buffer, MAX_BUFFER_SIZE - 1);
		if (ret < 0)
		{
			printf("cant get message from client .\n");
			return NULL;
		}

		if (ret == 0)
		{
			printf("server disconnected .\n");
			return NULL;
		}

		if (ret < MAX_BUFFER_SIZE)
		{
			buffer[ret] = '\0';
		}
		printf("server: %s", buffer);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t ts, tr;
	int port;
	struct sockaddr_in addr;

	if (argc < 1)
	{
		perror("format is ./server <port_number>");
		return -1;
	}

	port = atoi(argv[1]);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket creation failed");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	// addr.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET, "10.130.157.210", &addr.sin_addr);
	if ((connect(sockfd, (struct sockaddr *)(&addr), sizeof(addr))) < 0)
	{
		perror("unable to connect...\n");
		close(sockfd);
		return -1;
	}

	pthread_create(&ts, NULL, sendt, NULL);
	pthread_create(&tr, NULL, recvt, NULL);

	pthread_join(ts, NULL);
	pthread_join(tr, NULL);

	close(sockfd);
	return 0;
}
