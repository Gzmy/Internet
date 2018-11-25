#include "udp_server.h"
#include <pthread.h>

void *recvMsg(void *arg)
{
    server *s = (server*)arg;

    string str;
    for(;;){
        s->recvData(str);
        cout << "debug:" << str << endl;
    }
}

void *sendMsg(void *arg)
{
    server *s = (server*)arg;
    for(;;){
        s->broadcastData(); //广播
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        cout << "Usage: " << argv[0] << "-port" << endl;
        return 1;
    }
    int port = atoi(argv[1]);

    server s(port);
    s.serverInit();

    
    pthread_t recver, sender;
    pthread_create(&recver, NULL, recvMsg, (void *)&s);
    pthread_create(&sender, NULL, sendMsg, (void *)&s);

    pthread_join(recver, NULL);
    pthread_join(sender, NULL);
}
