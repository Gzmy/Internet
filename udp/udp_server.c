/*
 * 这是一个简单的udp服务程序,可以显示从客户端发来的数据
 * 1.创建套接字
 * 2.绑定Ip和端口
 * 3.接收从客户端发来的数据
 * 4.发送接收的数据
 *
 */


#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("Usage: %s [ip] [prot]\n", argv[0]);
		return 1;
	}

	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//返回值一个文件描述符
	if(sock < 0)
	{
		perror("socket");
		return 2;
	}

	struct sockaddr_in local;
	local.sin_family = AF_INET;//协议族
	local.sin_port = htons(atoi(argv[2]));//将本地端口号转至网络端口号
	local.sin_addr.s_addr = inet_addr(argv[1]);//将192.168.34.34转成四字节

	//绑定Ip和端口号
	if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
		perror("bind");
		return 3;
	}

	//提供服务, 接受数据
	char buf[1024];
	while(1)
	{
		struct sockaddr_in client;//收数据的套接字
		socklen_t len = sizeof(client);
		ssize_t s = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr *)&client, &len);//接收到了客户端的Ip和port,以便下面sendto发往客户端.接收的时候靠sock
		if(s > 0)
		{
			buf[s] = 0;
			printf("[%s:%d] : %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buf);
			fflush(stdout);

			//发送数据
			sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&client, sizeof(client));
		}
	}

	close(sock);
	return 0;
}
