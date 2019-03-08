#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static const char *request = "GET http://www.baidu.com/index.html HTTP/1.0\r\nConnection: keep-alive\r\n\r\nxxxxxxxxxxxx";

void setnonBlock(int fd) {
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
}

void addfd(int epoll_fd, int sock_fd) {
	struct epoll_event event;
	event.data.fd = sock_fd;
	event.events = EPOLLOUT | EPOLLET | EPOLLERR;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event);
	setnonBlock(sock_fd);
}

bool write_nbyte(int sock, const char* buff, int len) {
	int writeSize = 0;
	printf("write out %d byte to socket %d\n", len, sock);
	while(1) {
		writeSize = send(sock, buff, len ,0);
		if(writeSize == -1) {
			return false;
		} else if(writeSize == 0) {
			return false;
		}

		len -= writeSize;
		buff = buff + writeSize;
		if(len <= 0) {
			return true;
		}
	}
}

bool read_once(int sock_fd, char *buff, int len) {
	int readSize = 0;
	memset(buff, '\0', len);
	readSize = recv(sock_fd, buff, len, 0);
	if(readSize == -1) {
		return false;
	} else if(readSize == 0) {
		return false;
	}
	printf("read in %d byte from socket %d with connect: %s\n", readSize, sock_fd, buff);
	return true;
}

//向服务器发起num个tcp连接，
void start_conn(int epoll_fd, int num, const char *ip, int port) {
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);
	socklen_t len = sizeof(local);

	for(int i = 0; i < num; i++) {
		sleep(1);
		int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
		printf("create one sock\n");
		if(sock_fd < 0) {
			continue;
		} 
		if(connect(sock_fd, (struct sockaddr*)&local, len) == 0) {
			printf("build connection %d\n", i);
			addfd(epoll_fd, sock_fd);
		}
	}
}

void close_conn(int epoll_fd, int sock_fd) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_fd, 0);
	close(sock_fd);
}

int main(int argc, char *argv[])
{
	assert(argc == 4);
	int epoll_fd = epoll_create(256);
	start_conn(epoll_fd, atoi(argv[3]), argv[1], atoi(argv[2]));
	struct epoll_event events[1000];
	char buffer[2048];
	while(1) {
		int fds = epoll_wait(epoll_fd, events, 1000, 2000);
		for(int i = 0; i < fds; i++) {
			int sock_fd = events[i].data.fd;
			if(events[i].events & EPOLLIN) {
				if(!read_once(sock_fd, buffer, 2048)) {
					close_conn(epoll_fd, sock_fd);
				}
				struct epoll_event event;
				event.data.fd = sock_fd;
				event.events = EPOLLOUT | EPOLLET | EPOLLERR;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sock_fd, &event);
			} else if(events[i].events & EPOLLOUT) {
				if(!write_nbyte(sock_fd, request, strlen(request))) {
					close_conn(epoll_fd, sock_fd);
				}
				struct epoll_event event;
				event.data.fd = sock_fd;
				event.events = EPOLLIN | EPOLLET | EPOLLERR;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sock_fd, &event);
			} else if(events[i].events & EPOLLERR) {
				close_conn(epoll_fd, sock_fd);
			}
		}
	}
	return 0;
}
