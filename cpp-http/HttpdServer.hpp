#ifndef __HTTPD_SERVER_HPP__
#define __HTTPD_SERVER_HPP__

#include <pthread.h>
#include "ProtocolUtil.hpp"
#include "ThreadPool.hpp"

class HttpdServer{
public:
    HttpdServer(int port_) : port(port_), listen_sock(-1), tp(NULL)
    {}

    void InitServer()
    {
        listen_sock = socket(AF_INET, SOCK_STREAM, 0);
        if(listen_sock < 0){
            LOG(ERROR, "create socket error");
            exit(2);
        }

        int opt_ = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt_, sizeof(opt_));

        struct sockaddr_in local_;
        local_.sin_family = AF_INET;
        local_.sin_port = htons(port);
        local_.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(listen_sock, (struct sockaddr*)&local_, sizeof(local_)) < 0){
            LOG(ERROR, "bind socket error");
            exit(3);
        }

        if(listen(listen_sock, 5) < 0){ //缓冲队列，缓解峰值压力
            LOG(ERROR, "listen socket error");
            exit(4);
        }

        LOG(INFO, "initServer success!");

        tp = new ThreadPool();
        tp->initThreadPool();
    }

    void Start()
    {
        LOG(INFO, "Start Server begin");
        for( ; ; ){
            struct sockaddr_in peer_;
            socklen_t len = sizeof(peer_);
            int sock_ = accept(listen_sock, (struct sockaddr*)&peer_, &len);
            if(sock_ < 0){
                LOG(WARNING, "accept error");
                continue;
            }

            LOG(INFO, "Get New Client, Handler request....");
            Task t_;
            t_.SetTask(sock_, Entry::HandlerRequest);
            tp->PushTask(t_);
        }
    }

    ~HttpdServer()
    {
        if(listen_sock != -1){
            close(listen_sock);
        }
        port = -1;
    }
private:
    int port;
    int listen_sock;
    ThreadPool *tp;
};

#endif
