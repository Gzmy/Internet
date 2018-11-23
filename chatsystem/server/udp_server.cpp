#include "udp_server.h"

server::server(int _port)
{
    port = _port;
}

void server::serverInit()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        cerr << "socket error" << endl;
        exit(1);
    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0){
        cerr << "bind error" << endl;
        exit(2);
    }
}

void server::recvData(string &out)
{
    char buf[MAX];
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    ssize_t s = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&peer, &len);
    if(s > 0){
        buf[s] = 0;
        out = buf;
        pool.putData(out);
    }
}

void server::sendData(const string &in, struct sockaddr_in &peer)
{
    sendto(sock, in.c_str(), in.size(), 0, (struct sockaddr*)&peer, sizeof(peer));
}

void server::broadcastData()
{
    string str;
    pool.getData(str);
    //server::sendData(str, struct sockaddr_in &peer);

}

server::~server()
{
    close(sock);
}
