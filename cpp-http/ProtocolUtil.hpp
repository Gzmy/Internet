#ifndef __PROTOCOL_UTIL_HPP__
#define __PROTOCOL_UTIL_HPP__
//处理逻辑

#include <iostream>
#include <string>
#include <string.h>
#include <strings.h>
#include <sstream>
#include <unordered_map>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Log.hpp"

#define NOT_FOUND 404
#define OK 200

#define WEB_ROOT "wwwroot"
#define HOME_PAGE "index.html"

#define HTTP_VERSION "HTTP/1.0"

std::unordered_map<std::string, std::string> stuffix_map{
    {".html", "text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},
    {".js", "application/x-javascript"}
};

//字符串解析
class ProtocolUtil{
public:
    static void MakeKV(std::unordered_map<std::string, std::string> &kv_, std::string &str_)
    {
        std::size_t pos_ = str_.find(": ");
        if(std::string::npos == pos_){
            return;
        }

        std::string k_ = str_.substr(0, pos_);
        std::string v_ = str_.substr(pos_+2);

        kv_.insert(make_pair(k_, v_));
    }

    static std::string IntToString(int code_)
    {
        std::stringstream ss;
        ss << code_;
        
        return ss.str();
    }

    static std::string CodeToDesc(int code_)
    {
        switch(code_){
        case 200:
            return "OK";
        case 404:
            return "NOT_FOUND";
        default:
            return "UNKNOW";
        }
    }
    
    static std::string SuffixToType(const std::string &suffix_)
    {
        return stuffix_map[suffix_];
    }
};

//请求
class Request{
public:
    Request() : blank("\n") , cgi(false) , path(WEB_ROOT), resource_size(0), content_length(-1), resource_suffix(".html")
    {}

    void RequestLineParse()
    {
        //GET / HTTP/1.0
        std::stringstream ss(rq_line);
        ss >> method >> uri >> version;
    }

    void UriParse() //解析uri
    {
        if(strcasecmp(method.c_str(), "GET") == 0){
            std::size_t pos_ = uri.find('?');
            if(std::string::npos != pos_){ //有'?', 是cgi
                cgi = true;
                path += uri.substr(0, pos_);
                param = uri.substr(pos_+1);
            }else{
                path += uri;
            }
        }else{
            path += uri;
        }

        if(path[path.size() - 1] == '/'){
            path += HOME_PAGE;
        }
    }

    bool RequestHeadParse()
    {
        int start_ = 0;
        while(start_ < rq_head.size()){
            std::size_t pos_ = rq_head.find('\n', start_);
            if(std::string::npos == pos_){
                break;
            }

            std::string sub_string_ = rq_head.substr(start_, pos_ - start_);
            if(!sub_string_.empty()){
                LOG(INFO, "substr is not empty");
                ProtocolUtil::MakeKV(head_kv, sub_string_);
            }else{
                LOG(INFO, "substr is  empty");
                break;
            }
            start_ = pos_ + 1;
        }
        return true;
    }

    int GetContentLength()
    {
        std::string cl_ = head_kv["Content-Length"];
        if(!cl_.empty()){
            std::stringstream ss(cl_);
            ss >> content_length;
        }
        return content_length;
    }

    std::string &GetParam()
    {
        return param;
    }

    std::string &GetSuffix()
    {
        return resource_suffix;
    }

    std::string &GetPath()
    {
        return path;
    }

    bool IsMethodLegal()
    {
        if(strcasecmp(method.c_str(), "GET") == 0 || (cgi = (strcasecmp(method.c_str(), "POST") == 0))){
            return true;
        }
        return false;
    }

    bool IsPathLegal() //判断资源是否存在
    {
        struct stat st;
        if(stat(path.c_str(), &st) < 0){
            LOG(WARNING, "path not found");
            return false;
        }

        if(S_ISDIR(st.st_mode)){ //is a dictory
            path += "/";
            path += HOME_PAGE;
        }else{ // is have execmode?
            if((st.st_mode & S_IXUSR) ||
               (st.st_mode & S_IXGRP) ||
               (st.st_mode & S_IXOTH) ){
                cgi = true;
            }
        }

        resource_size = st.st_size;
        std::size_t pos = path.rfind(".");
        if(std::string::npos != pos){
            resource_suffix = path.substr(pos);
        }

        return true;
    }

    bool IsNeedRecvText()
    {
        if(strcasecmp(method.c_str(), "POST") == 0){
            return true;
        }else{
            return false;
        }
    }

    bool IsCgi()
    {
        return cgi;
    }

    int GetResourceSize()
    {
        return resource_size;
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

    int resource_size; //资源大小
    std::string resource_suffix; //请求格式
    int content_length; //请求正文长度
    std::unordered_map<std::string, std::string> head_kv;
};

//响应
class Response{
public:
    Response() : blank("\n") , code(OK), fd(-1)
    {}

    void MakeStatusLine()
    {
        rsp_line = HTTP_VERSION;
        rsp_line += " ";
        rsp_line += ProtocolUtil::IntToString(code);
        rsp_line += " ";
        rsp_line += ProtocolUtil::CodeToDesc(code);
        rsp_line += "\n";
    }

    void MakeResponseHead(Request *&rq_)
    {
        rsp_head = "Content-Length: ";
        rsp_head += ProtocolUtil::IntToString(rq_->GetResourceSize());
        rsp_head += "\n";
        
        rsp_head += "Content-Type: ";
        std::string suffix_ = rq_->GetSuffix();
        rsp_head += ProtocolUtil::SuffixToType(suffix_);
        rsp_head += "\n";
    }

    void OpenResource(Request *&rq_)
    {
        std::string path_ = rq_->GetPath();
        fd = open(path_.c_str(), O_RDONLY);
    }

    ~Response()
    {
        if(fd >= 0){
            close(fd);
        }
    }
public:
    int code;
    int fd;
public:
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

    void RecvRequestHead(std::string &head_)
    {
        head_ = "";
        std::string line_;
        while(line_ != "\n"){
            line_ = "";
            RecvOneLine(line_);
            head_ += line_;
        }
    }

    void RecvRequestText(std::string &text_, int len_, std::string &param_)
    {
        char c_;
        int i_ = 0;
        while(i_ < len_){
            recv(sock, &c_, 1, 0);
            text_.push_back(c_);
            i_++;
        }
        param_ = text_;
    }

    void SendResponse(Response *&rsp_, Request *&rq_, bool cgi_)
    {
        if(cgi_){

        }else{
            std::string &rsp_line_ = rsp_->rsp_line;
            std::string &rsp_head_ = rsp_->rsp_head;
            std::string &blank_ = rsp_->blank;
            int &fd = rsp_->fd;

            send(sock, rsp_line_.c_str(), rsp_line_.size(), 0);
            send(sock, rsp_head_.c_str(), rsp_head_.size(), 0);
            send(sock, blank_.c_str(), blank_.size(), 0);
            sendfile(sock, fd, NULL, rq_->GetResourceSize());
        }
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
    static void ProcessNonCgi(Connect *&conn_, Request *&rq_, Response *&rsp_)
    {
        int &code_ = rsp_->code;
        rsp_->MakeStatusLine();
        rsp_->MakeResponseHead(rq_);
        rsp_->OpenResource(rq_);
        conn_->SendResponse(rsp_, rq_, false);
    }

    static void ProcessCgi(Connect *&conn_, Request *&rq_, Response *&rsp_)
    {
        int &code_ = rsp_->code;
        int input[2];
        int output[2];

        //站在子进程角度，子进程通过input读
        pipe(input);
        pipe(output);

        pid_t id = fork();
        if(id < 0){
            LOG(ERROR, "fork error");
            code_ = NOT_FOUND;
            return;
        }
        else if(id == 0){ //child
            close(input[1]);
            close(output[0]);
        }else{ //father
            close(input[0]);
            close(output[1]);
        }
    }

    static int ProcessResponse(Connect *&conn_, Request *&rq_, Response *&rsp_)
    {
        if(rq_->IsCgi()){
            ProcessCgi(conn_, rq_, rsp_);
        }else{
            ProcessNonCgi(conn_, rq_, rsp_);
        }
    }
    
    static void *HandlerRequest(void *arg_)
    {
        int sock_ = *(int*)arg_;
        delete (int*)arg_;
        Connect *conn_ = new Connect(sock_);
        Request *rq_ = new Request();
        Response *rsp_ = new Response();
    
        int &code_ = rsp_->code;

        conn_->RecvOneLine(rq_->rq_line); //读取一行
        rq_->RequestLineParse(); //解析请求行,分离,方法,uri和版本
        if(!rq_->IsMethodLegal()){ //判断方法是否合法
            code_ = NOT_FOUND;
            goto end;
        }

        rq_->UriParse(); //解析uri
        if(!rq_->IsPathLegal()){ //访问资源不存在
            code_ = NOT_FOUND;
            goto end;
        }

        LOG(INFO, "request path is ok");

        conn_->RecvRequestHead(rq_->rq_head); //读取请求头
        if(rq_->RequestHeadParse()){ //解析报头
            LOG(INFO, "paser head done");
        }else{
            code_ = NOT_FOUND;
            goto end;
        }

        if(rq_->IsNeedRecvText()){
            conn_->RecvRequestText(rq_->rq_text, rq_->GetContentLength(), rq_->GetParam());
        }
        //request recv done!

        ProcessResponse(conn_, rq_, rsp_); //执行响应
end:
        if(code_ != OK){
            //HadnlerError(sock_); //处理异常
        }
        delete conn_;
        delete rq_;
        delete rsp_;
    }
};

#endif
