#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("Usage %s [Ip] [Port]\n", argv[0]);
		return 1;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		perror("socket");
		return 1;
	}

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));
	local.sin_addr.s_addr = inet_addr(argv[1]);

	if(bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
		perror("bind");
		return 1;
	}

	if(listen(fd, 5) < 0)
	{
		perror("listen");
		return 1;
	}

	for(;;)
	{
		struct sockaddr_in client_addr;
		socklen_t len;
		int sock = accept(fd, (struct sockaddr*)&client_addr, &len);
		if(sock < 0)
		{
			perror("accept");
			continue;
		}
		char input_buf[1024 * 10] = {0};
		ssize_t read_size = read(sock, input_buf, sizeof(input_buf)-1);
		if(read_size < 0)
			return 1;
		printf("[Request] %s", input_buf);
		char buf[1024] = {0};
		const char *hello = "<h1>I Love You, baby</h1>";
		sprintf(buf, "HTTP/1.0 200 OK\nContent-Length:%lu\n\n%s", strlen(hello), hello);
		write(sock, buf, strlen(buf));
	}
	return 0;
}
