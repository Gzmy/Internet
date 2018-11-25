#include "base_json.h"

void stringToValue(string &inString, Json::Value &root) //反序列  1->4
{
    Json::Reader r;
    r.parse(inString, root, false);
}

void valueToString(Json::Value &root, string &outString) //序列化 4->1
{
    Json::FastWriter w;
    outString = w.write(root);
}
