/*
 * 这是一个简单tcp服务端程序,实现一个简单的聊天程序
 * tcp是有连接的面向字节流的可靠传输
 * 1.创建socket
 * 2.绑定地址
 * 3.开始监听,listen,可以开始接收客户端连接请求
 * 4.获取建立成功的socket,最新的socket
 * 5.接收消息
 * 6.发送消息
 * 7.关闭socket
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

void serviceIO(int sock)
{
	char buf[1024];
	while(1)
	{
		ssize_t s = read(sock, buf, sizeof(buf)-1);
		if(s > 0)
		{
			buf[s] = 0;
			printf("client# %s\n", buf);
			//write(sock, buf, strlen(buf));
		}
		else if(s == 0)//对端关闭连接
		{
			printf("client quit!\n");
			break;
		}
		else
		{
			perror("read");
			break;
		}
	}
	close(sock);
}

void *service(void *arg)
{
	int sock = *(int *)arg;
	serviceIO(sock);
}

//./tcp_sercer ip port
int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("Usage: %s [Ip] [Port]\n", argv[0]);
		return 1;
	}

	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock < 0)
	{
		perror("socket");
		return 2;
	}

	int opt = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//设置地址复用

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));
	local.sin_addr.s_addr = inet_addr(argv[1]);
	
	if(bind(listen_sock, (struct sockaddr *)&local, sizeof(local)) < 0)
	{
		perror("bind");
		return 3;
	}

	if(listen(listen_sock, 5) < 0)
	{
		perror("listen");
		return 4;
	}

	while(1)
	{
		struct sockaddr_in client;
		socklen_t len = sizeof(client);
		int sock = accept(listen_sock, (struct sockaddr *)&client, &len);
		if(sock < 0)
		{
			perror("accept");
			continue;
		}
		printf("get new link [%s : %d]..!\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		//多线程
		pthread_t id;
		pthread_create(&id, NULL, service, (void *)&sock);
		pthread_detach(id);

		//多进程
		//pid_t id = fork();
		//if(id == 0)
		//{
		//	close(listen_sock);
		//	if(fork() > 0)
		//		exit(0);
		//	serviceIO(sock);//孤儿
		//	exit(0);
		//}
		//else
		//{
		//	close(sock);
		//	waitpid(id, NULL, 0);//阻塞方式等待子进程返回
		//}
	}

	return 0;
}


//int main(int argc, char *argv[])
//{
//	if(argc != 3)
//	{
//		printf("Usage : %s [Ip] [port]\n", argv[0]);
//		return 1;
//	}
//	
//	int sock = socket(AF_INET, SOCK_STREAM, 0);
//	if(sock < 0)
//	{
//		perror("socket");
//		return -1;
//	}
//
//	struct sockaddr_in local;
//	local.sin_family = AF_INET;
//	local.sin_port = htons(atoi(argv[2]));
//	local.sin_addr.s_addr = inet_addr(argv[1]);
//	socklen_t len = sizeof(local);	
//
//	if(bind(sock, (struct sockaddr *)&local, len) < 0)
//	{
//		perror("bind");
//		return -2;
//	}
//
//	if(listen(sock, 5) < 0)
//	{
//		perror("listen");
//		return -3;
//	}
//
//	while(1)
//	{
//		//建立新连接
//		struct sockaddr_in cli_addr;
//		len = sizeof(cli_addr);
//		int new_sock = accept(sock, (struct sockaddr *)&cli_addr, &len);
//		if(new_sock < 0)
//		{
//			perror("accept");
//			continue;//获取失败再次尝试获取
//		}
//		printf("connect success : [%s][%d]\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
//
//		//接收数据
//		char buf[1024] = {0};
//		ssize_t s = recv(new_sock, buf, sizeof(buf)-1, 0);
//		if(s < 0)
//		{
//			perror("recv");
//			close(new_sock);
//			continue;
//		}
//		else if(s == 0)//连接中断
//		{
//			printf("peer shutdown!\n");
//			close(new_sock);
//			continue;
//		}
//		printf("[%s:%d] : %s\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);
//
//		//发送数据
//		memset(buf, 0x00, sizeof(buf));
//		scanf("%s", buf);
//		send(new_sock, buf, strlen(buf), 0);
//	}
//	close(sock);
//	return 0;
//}
