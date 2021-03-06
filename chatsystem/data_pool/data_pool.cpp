#include "data_pool.h"

dataPool::dataPool(int _cap) : cap(_cap), pool(_cap)
{
    product_step = 0;
    consume_step = 0;
    sem_init(&blankSem, 0, _cap); //生产者容量是_cap
    sem_init(&dataSem, 0, 0); //消费者没有东西
}

void dataPool::putData(const string &data) //放数据
{
    sem_wait(&blankSem);
    pool[product_step] = data;
    product_step++;
    product_step %= cap;
    sem_post(&dataSem);
}

void dataPool::getData(string &data) //拿数据
{
    sem_wait(&dataSem);
    data = pool[consume_step];
    consume_step++;
    consume_step %= cap;
    sem_post(&blankSem);
}

dataPool::~dataPool()
{
    sem_destroy(&blankSem);
    sem_destroy(&dataSem);
}

