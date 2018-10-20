/*
 *	这是一个tcp客户端简单聊天程序
 * 1.创建socket
 * 2.绑定地址
 * 3.发起连接请求
 * 4.发送数据
 * 5.接收数据
 * 6.关闭
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>


//int main(int argc, char *argv[])
//{
//	if(argc != 3)
//	{
//		printf("Usage : %s [ip] [port]", argv[0]);
//		return 1;
//	}
//
//	int sock = socket(AF_INET, SOCK_STREAM, 0);
//	if(sock < 0)
//	{
//		perror("socket");
//		return 2;
//	}
//
//	struct sockaddr_in server;
//	server.sin_family = AF_INET;
//	server.sin_port = htons(atoi(argv[2]));
//	server.sin_addr.s_addr = inet_addr(argv[1]);
//
//	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
//	{
//		perror("connet");
//		return 3;
//	}
//
//	char buf[1024];
//	while(1)
//	{
//		printf("Please Enter: ");
//		scanf("%s", buf);
//		write(sock, buf, strlen(buf));
//	}
//	return 0;
//}

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("Usage : %s [IP][port]", argv[0]);
		return 1;
	}
	//1.新建
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock < 0)
	{
		perror("socket");
		return -1;
	}

	//2.绑定:客户端不推荐绑定
	//3.向服务端发起连接请求
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	socklen_t len = sizeof(struct sockaddr_in);
	int ret = connect(sock, (struct sockaddr *)&serv_addr, len);
	if(ret < 0)
	{
		perror("connect error");
		return -1;
	}

	while(1)
	{
		//4.发送数据
		char buf[1024] = {0};
		scanf("%s", buf);
		send(sock, buf, strlen(buf), 0);
		//5.接收
		memset(buf, 0x00, 1024);
		ssize_t rlen = recv(sock, buf, sizeof(buf)-1, 0);
		if(rlen < 0)
		{
			perror("recv");
			close(sock);
			return -1;
		}
		else if(rlen == 0)
		{
			printf("perr shutdown!\n");
			close(sock);
			continue;
		}
		printf("server echo:%s\n", buf);
	}
	return 0;
}
