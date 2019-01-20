#define _GNU_SOURCE 1 //?????
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>

#define MAX_SIZE 64

int main(int argc, char *argv[])
{
	if(argc != 2) {
		printf("usage: %s port\n", argv[0]);
		return 1;
	}

	int port = atoi(argv[1]);

	struct sockaddr_in local;
	bzero(&local, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(port);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd >= 0);

	if(connect(sockfd, (struct sockaddr*)&local, sizeof(local)) < 0) {
		perror("connect");
		close(sockfd);
		return 1;
	}

	struct pollfd fds[2];
	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	fds[1].fd = sockfd;
	fds[1].events = POLLIN | POLLRDHUP;
	fds[1].revents = 0;

	char read_buf[MAX_SIZE];
	int pipefd[2];
	int ret = pipe(pipefd);
	assert(ret != -1);

	while(1) {
		ret = poll(fds, 2, -1);
		if(ret < 0) {
			printf("poll failure\n");
			break;
		}

		if(fds[1].revents & POLLRDHUP) {
			printf("server closed connect");
			break;
		} else if (fds[1].revents & POLLIN) {
			memset(read_buf, '\0', MAX_SIZE);
			recv(fds[1].fd, read_buf, MAX_SIZE-1, 0);
			printf("%s\n", read_buf);
		}

		if(fds[0].revents & POLLIN) {
			ret = splice(0, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
			ret = splice(pipefd[0], NULL, sockfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
		}
	}
	close(sockfd);
	return 0;
}
