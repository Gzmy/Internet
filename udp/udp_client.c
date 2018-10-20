/*
 * 这是一个简单的udp客户端程序
 * 1.创建套接字
 * 2.绑定由操作系统自主绑定
 * 3.发送数据
 * 4.接收数据
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>

//./udp_client serverIP serverPort
int main(int argc, char *argv[])
{	
	if(argc != 3)
	{
		printf("Usage : %s[server_ip] [server_port]\n", argv[0]);
		return 1;
	}

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
	{
		perror("socket");
		return 2;
	}
	

	char buf[1024];
	while(1)
	{
		struct sockaddr_in local;
		local.sin_family = AF_INET;
		local.sin_port = htons(atoi(argv[2]));
		local.sin_addr.s_addr = inet_addr(argv[1]);

		printf("Please Enter: ");
		fflush(stdout);
		ssize_t s = read(0, buf, sizeof(buf)-1);
		if(s > 0)
		{
			buf[s] = 0;
			sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&local, sizeof(local));
			recvfrom(sock, buf, sizeof(buf)-1, 0, NULL, NULL);
			printf("server echo# %s\n", buf);
		}
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
//	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//	if(sock < 0)
//	{
//		perror("socket");
//		return 2;
//	}
//
//	struct sockaddr_in local;
//	socklen_t len = sizeof(local);
//	local.sin_family = AF_INET;
//	local.sin_port = htons(atoi(argv[2]));
//	local.sin_addr.s_addr = inet_addr(argv[1]);
//
//	while(1)
//	{
//		char buf[1024] = {0};
//		ssize_t s = read(0, buf, sizeof(buf)-1);
//		if(s < 0)
//		{
//			perror("read");
//			return 3;
//		}
//		else
//		{
//			buf[s-1] = 0;
//			sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&local, len);
//			s = recvfrom(sock, buf, sizeof(buf)-1, 0, NULL, NULL);
//			if(s > 0)
//			{
//				buf[s] = 0;
//				printf("server echo> %s \n", buf);
//			}
//		}
//	}
//
//	close(sock);
//
//	return 0;
//}
