#include "data_pool.h"

dataPool::dataPool(int _cap) : cap(_cap), pool(_cap)
{
    product_step = 0;
    consume_step = 0;
}

void dataPool::putData(const string &data)
{
    pool[product_step] = data;
    product_step++;
    product_step %= cap;
}

void dataPool::getData(string &data)
{
    data = pool[consume_step];
    consume_step++;
    consume_step %= cap;
}

dataPool::~dataPool()
{
}

