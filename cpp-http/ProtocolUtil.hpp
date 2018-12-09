#ifndef __PROTOCOL_UTIL_HPP__
#define __PROTOCOL_UTIL_HPP__
//处理逻辑

#include <iostream>
#include <string>
#include <strings.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Log.hpp"

#define NOT_FOUND 404
#define OK 200

#define WEB_ROOT "wwwroot"
#define HOME_PAGE "index.html"

//请求
class Request{
public:
    Request() : blank("\n") , cgi(false) , path(WEB_ROOT)
    {}

    void RequestLineParse()
    {
        //GET / HTTP/1.0
        std::stringstream ss(rq_line);
        ss >> method >> uri >> version;
    }

    void UriParse() //解析uri
    {
        std::size_t pos_ = uri.find('?');
        if(std::string::npos != pos_){ //有'?', 是cgi
            cgi = true;
            path += uri.substr(0, pos_);
            param = uri.substr(pos_+1);
        }else{
            path += uri;
        }
        if(path[path.size() - 1] == '/'){
            path += HOME_PAGE;
        }
    }

    bool IsMethodLegal()
    {
        if(strcasecmp(method.c_str(), "GET") == 0 || cgi = (strcasecmp(method.c_str() == "POST") == 0)){
            return true;
        }
        return false;
    }

    ~Request()
    {}
public:
    std::string rq_line; //请求行
    std::string rq_head; //请求报头
    std::string blank;   //空行
    std::string rq_text; //请求正文
private:
    std::string method;
    std::string uri;
    std::string version;
    bool cgi; //method=POST, GET->uri(?)
    std::string path; //资源路径
    std::string param; //请求参数
};

//响应
class Response{
public:
    Response() : blank("\n") , code(OK)
    {}
    ~Response()
    {}
public:
    int code;
private:
    std::string rsp_line;
    std::string rsp_head;
    std::string blank;
    std::string rsp_text;
};

//连接对象, 数据读写操作
class Connect{
public:
    Connect(int sock_) : sock(sock_)
    {}
    
    int RecvOneLine(std::string &line_) //读取一行
    {
        //三种换行符 \r  \n  \r\n   ------->  全部转成\n
        char c = '\0';
        while(c != '\n'){
            ssize_t s = recv(sock, &c, 1, 0);
            if(s > 0){
                if(c == '\r'){
                    recv(sock, &c, 1, MSG_PEEK);
                    if(c == '\n'){
                        recv(sock, &c, 1, 0); //覆盖\r\n  --> \n
                    }else{
                        c = '\n';  // \r --> \n
                    }
                }

                line_.push_back(c);
            }else{
                break;
            }
        }

        return line_.size();
    }

    ~Connect()
    {
        if(sock >= 0){
            close(sock);
        }
    }
private:
    int sock;
};

//处理
class Entry{
public:
    static void *HandlerRequest(void *arg_)
    {
        int sock_ = *(int*)arg_;
        delete (int*)arg_;
        Connect *conn_ = new Connect(sock_);
        Request *rq_ = new Request();
        Response *rsp_ = new Response();
    
        int &code_ = rsp_->code;

        conn_->RecvOneLine(rq_->rq_line); //读取一行
        rq_->RequestLineParse(); //解析请求行
        if(!rq_->IsMethodLegal()){ //判断方法是否合法
            code_ = NOT_FOUND;
            goto end;
        }

        rq_->UriParse(); //解析uri

end:
        if(code != OK){
            //HadnlerError(sock_); //处理异常
        }
        delete conn_;
        delete rq_;
        delete rsp_;
    }
};

#endif
