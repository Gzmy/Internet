/*
 *	这是一个多线程的tcp简单聊天程序  服务端
 *	1. 创建socket
 *	2. 绑定地址
 *	3. 开始监听
 *	4. 获取客户端的socket连接
 *	5. 创建线程，在线程中通过获取的socket连接与客户端通信
 *	6.关闭socket
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>

void *thr_start(void *arg)
{
	int sock = (int)arg;
	
	while(1)
	{
		char buf[1024] = {0};
		ssize_t ret = recv(sock, buf, sizeof(buf)-1, 0);
		if(ret <= 0)
		{
			perror("recv");
			close(sock);
			break;
		}
		printf("client say:[%s]\n", buf);
		send(sock, "what?", 5, 0);
	}
	return NULL;
}

void create_worker(int sock)
{
	//创建一个线程来单独处理与指定的客户端的通信
	pthread_t tid;
	int ret;

	ret = pthread_create(&tid, NULL, thr_start, (void*)sock);
	if(ret != 0)//失败
	{
		printf("create pthread error!\n");
		return;
	}
}

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("Usage : %s [Ip] [Port]\n", argv[0]);
		return 1;
	}

	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock < 0)
	{
		perror("socket");
		return 1;
	}

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));
	local.sin_addr.s_addr = inet_addr(argv[1]);
	socklen_t len = sizeof(local);

	if(bind(listen_sock, (struct sockaddr*)&local, len) < 0)
	{
		perror("bind");
		return -1;
	}

	if(listen(listen_sock, 5) < 0)
	{
		perror("listen");
		return -1;
	}

	char buf[1024];
	while(1)
	{
		struct sockaddr_in client;
		len = sizeof(client);
		int sock = accept(listen_sock, (struct sockaddr*)&client, &len);
		if(sock < 0)
		{
			perror("accept");
			continue;
		}
		printf("connect success : [%s] [%d]\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		create_worker(sock);
	}
	return 0;
}
