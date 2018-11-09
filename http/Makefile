WORK_DIR=$(shell pwd)
cc=gcc
bin=httpd
src=httpd.c
LDFLAGS=-lpthread

.PHONY:all
all:$(bin) cgi

$(bin):$(src)
	$(cc) -o $@ $^ $(LDFLAGS) -w

cgi:
	cd wwwroot/cgi; make clean; make; cd -
	cd sql_connect; make clean; make; cd -

.PHONY:clean
clean:
	rm -f $(bin)
	cd wwwroot/cgi; make clean; cd -
	cd sql_connect; make clean; cd -
	rm -rf output

.PHONY:output
output:
	mkdir -p output/wwwroot/imgs
	mkdir -p output/wwwroot/cgi
	mkdir -p output/lib
	cp httpd output
	cp -rf lib/lib/* output/lib
	cp wwwroot/*.html output/wwwroot
	cp wwwroot/imgs/* output/wwwroot/imgs
	cp wwwroot/cgi/math_cgi output/wwwroot/cgi
	cp sql_connect/insert_cgi output/wwwroot/cgi
	cp sql_connect/select_cgi output/wwwroot/cgi









