#pragma once

#include <iostream>
using namespace std;
#include <vector>
#include <string>

class dataPool{
public:
    dataPool(int _cap);
    void putData(const string &data);
    void getData(string &data);
    ~dataPool();
private:
    vector<string> pool;
    int cap;
    int product_step;
    int consume_step;
};
