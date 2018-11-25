#include "data.h"

data::data()
{}

void data::serialize(string &outString) //序列化  4->1
{
    Json::Value root;
    root["nick_name"] = nick_name;
    root["school"] = school;
    root["message"] = message;
    root["command"] = command;

    valueToString(root, outString);
}

void data::unserialize(string &inString) //反序列化  1->4
{
    Json::Value root;
    stringToValue(inString, root);

    nick_name = root["nick_name"].asString();
    school = root["school"].asString();
    message = root["message"].asString();
    command = root["command"].asString();
}

data::~data()
{}
