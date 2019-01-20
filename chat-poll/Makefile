.PHONY:all clean

all:chat_cli chat_sev

chat_cli:chat_cli.cc
	g++ -o $@ $^

chat_sev:chat_sev.cc
	g++ -o $@ $^

clean:
	rm chat_cli chat_sev
