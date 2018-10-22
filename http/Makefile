WORK_DIR=$(shell pwd)
cc=gcc
bin=httpd
src=httpd.c
LDFLAGS=-lpthread


$(bin):$(src)
	$(cc) -o $@ $^ $(LDFLAGS) -w

.PHONY:clean
clean:
	rm -f $(bin)
