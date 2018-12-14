#include <iostream>
#include <cstdio>
#include <string>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    if(getenv("Content-Length")){
        int size_ = atoi(getenv("Content-Length"));
        std::string param_;
        char c;
        while(size_){
            read(0, &c, 1);
            param_.push_back(c);
            size_--;
        }

        int x, y;
        sscanf(param_.c_str(), "x=%d&y=%d", &x, &y);

        std::cout << "<html>" << std::endl;
        std::cout << "<head>My Cal</head>" << std::endl;
        std::cout << "<body>" << std::endl;
        std::cout << "<h3>" << x << "+" << y << "=" << x+y << "</h3>" << std::endl;
        std::cout << "<h3>" << x << "-" << y << "=" << x-y << "</h3>" << std::endl;
        std::cout << "<h3>" << x << "*" << y << "=" << x*y << "</h3>" << std::endl;
        if(y == 0){
            std::cout << "<h3>" << x << "/" << y << "=" << -1 << "(div zero)</h3>" << std::endl;
            std::cout << "<h3>" << x << "%" << y << "=" << -1 << "(div/zero)</h3>" << std::endl;
        }else{
            std::cout << "<h3>" << x << "/" << y << "=" << x/y << "</h3>" << std::endl;
            std::cout << "<h3>" << x << "%" << y << "=" << x%y << "</h3>" << std::endl;
        }
        std::cout << "</body>" << std::endl;
        std::cout << "</html>" << std::endl;
    }
    return 0;
}
