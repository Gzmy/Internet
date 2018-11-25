#pragma once

#include <iostream>
using namespace std;
#include <vector>
#include <string>
#include <semaphore.h>

#define NUM 128

class dataPool{
public:
    dataPool(int _cap = NUM);
    void putData(const string &data);
    void getData(string &data);
    ~dataPool();
private:
    vector<string> pool;
    int cap;
    int product_step;
    int consume_step;
    sem_t blankSem; //生产者
    sem_t dataSem; //消费者
};
