/*
 * 修改文件描述符为非阻塞
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void setNonBlock(int fd)
{
	int fl = fcntl(fd, F_GETFL);
	if(fl < 0){
		perror("fcntl");
		return;
	}

	fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

void my_accept(int listen_sock, int epfd, struct epoll_event ev)
{
	while(1){
		struct sockaddr_in client;
		socklen_t len = sizeof(client);

		int new_sock = accept(listen_sock, (struct sockaddr*)&client, &len);
		if(new_sock < 0){
			break;
		}
		printf("get a new client!\n");

		//添加新事件到epoll模型

		setNonBlock(new_sock);
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = new_sock;
		epoll_ctl(epfd, EPOLL_CTL_ADD, new_sock, &ev);
	}
}

ssize_t my_read(int sock, char outBuf[], int size)
{
	ssize_t total = 0;
	ssize_t s = 0;
	while((s = read(sock, outBuf + total, 128)) > 0 && s ==128 && total < size){
		total += s;
	}
	outBuf[total] = 0;
	return total;
}

void handlerReadyEvents(int epfd, struct epoll_event revs[], int num, int listen_sock)
{
	int i = 0;

	struct epoll_event ev;
	for(; i < num; i++){
		int sock = revs[i].data.fd;
		uint32_t events = revs[i].events;

		if(sock == listen_sock && (events & EPOLLIN)){ //link event ready
			my_accept(sock, epfd, ev); //从底层不断获取连接
		}
		else if(events & EPOLLIN){ //normal read event ready
			char buf[10240];
			ssize_t s = my_read(sock, buf, sizeof(buf)-1);
			if(s > 0){
				buf[s] = 0;
				printf("%s", buf); //read ok
				ev.events = EPOLLOUT | EPOLLET;
				ev.data.fd = sock;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &ev);
			}else if(s == 0){
				printf("client quit!\n");
				close(sock);
				epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
			}else{
				perror("read");
			}
		}
		else if(events & EPOLLOUT){ //normal write event ready
			const char *echo_http = "http/1.0 200 OK\r\n\r\n<html><h1>hello Epoll Server!</h1></html>\r\n";
			write(sock, echo_http, strlen(echo_http));
			//my_write(sock, echo_http, strlen(echo_http));
			close(sock);
			epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
		}
		else{ //bug!
		}
	}
}

int startup(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0){
		perror("socket");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	setNonBlock(sock);

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0){
		perror("bind");
		exit(3);
	}

	if(listen(sock, 5) < 0){
		perror("bind");
		exit(4);
	}

	return sock;
}

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("Usage : %s [port]\n", argv[0]);
		return 1;
	}

	int listen_sock = startup(atoi(argv[1]));
	int epfd = epoll_create(256);
	if(epfd < 0){
		printf("epoll_create error!\n");
		return 5;
	}

	struct epoll_event ev; //需要监听的事
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = listen_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &ev);

	struct epoll_event revs[64]; //发生的事件存放在这个数组中
	while(1){
		int timeout = -1;
		int num = epoll_wait(epfd, revs, sizeof(revs)/sizeof(revs[0]), timeout);
		switch(num){
			case -1:
				printf("epoll_wait error!\n");
				break;
			case 0:
				printf("timeout!!!\n");
				break;
			default:
				handlerReadyEvents(epfd, revs, num, listen_sock);
				break;
		}
	}
}
