#pragma once

#include <iostream>
#include <string>
#include "base_json.h"
using namespace std;


//use json library
class data{
public:
    data();
    void serialize(string &outStrig); //序列化  4->1
    void unserialize(string &inString); //反序列化  1->4
    ~data();
public:
    string nick_name;
    string school;
    string message;
    string command; // 保留
};
