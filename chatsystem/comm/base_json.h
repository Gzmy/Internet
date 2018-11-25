#pragma once

#include <iostream>
#include <json/json.h>
using namespace std;

void stringToValue(string &inString, Json::Value &root);
void valueToString(Json::Value &root, string &outString);
