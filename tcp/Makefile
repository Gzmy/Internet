.PHONY:clean
.PHONY:all

all:tcp_server tcp_client

ttcp_server:ttcp_server.c
	gcc $^ -o $@ -lpthread

tcp_server:tcp_server.c
	gcc $^ -o $@ -lpthread

ptcp_server:ptcp_server.c
	gcc $^ -o $@ -lpthread

tcp_client:tcp_client.c
	gcc $^ -o $@

clean:
	rm tcp_server tcp_client

