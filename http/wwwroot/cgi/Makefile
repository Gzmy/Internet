.PHONY:clean

all:math_cgi test_cgi

math_cgi:math_cgi.c
	gcc $^ -o $@

test_cgi:test_cgi.c
	gcc $^ -o $@

clean:
	rm test_cgi math_cgi
