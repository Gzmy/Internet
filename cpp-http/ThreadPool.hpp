#ifndef __THREAD_POOL_HPP
#define __THREAD_POOL_HPP

#include <iostream>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include "Log.hpp"


typedef int (*handler_t)(int);

class Task{
public:
    Task()
    {
        sock = -1;
        handler = NULL;
    }

    void SetTask(int sock_, handler_t handler_)
    {
        sock = sock_;
        handler = handler_;
    }

    void Run()
    {
        handler(sock);
    }

    ~Task()
    {}
private:
    int sock;
    handler_t handler;
};

#define NUM 5

class ThreadPool{
public:
    ThreadPool(int num_ = NUM) : thread_total_num(num_), thread_idle_num(0), is_quit(false)
    {
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&cond, NULL);
    }

    void initThreadPool()
    {
        int i_ = 0;
        for(; i_ < thread_total_num; i_++){
            pthread_t tid;
            pthread_create(&tid, NULL, thread_routine, this);
        }
    }

    void PushTask(Task &t_)
    {
        LockQueue();
        if(is_quit){
            UnLockQueue();
            return;
        }
        task_queue.push(t_);
        WakeupOneThread();
        UnLockQueue();
    }

    void PopTask(Task &t_)
    {
        t_ = task_queue.front();
        task_queue.pop();
    }

    void Stop()
    {
        LockQueue();
        is_quit = true; //每个线程都要检测,临界资源，需要加锁
        UnLockQueue();

        while(thread_idle_num > 0){
            WakeupAllThread();
        }
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond);
    }

public:
    void LockQueue()
    {
        pthread_mutex_lock(&lock);
    }

    void UnLockQueue()
    {
        pthread_mutex_unlock(&lock);
    }

    bool IsEmpty()
    {
        return task_queue.empty();
    }

    void WakeupOneThread()
    {
        pthread_cond_signal(&cond);
    }

    void WakeupAllThread()
    {
        pthread_cond_broadcast(&cond);
    }

    void ThreadIdle()
    {
        if(is_quit){
            UnLockQueue();
            LOG(INFO, "thread quit....");
            thread_total_num--;
            pthread_exit((void*)1);
        }
        thread_idle_num++;
        pthread_cond_wait(&cond, &lock);
        thread_idle_num--;
    }

    static void *thread_routine(void *arg)
    {
        ThreadPool *tp_ = (ThreadPool*)arg;
        pthread_detach(pthread_self());
        for( ; ; ){
           tp_-> LockQueue();
            while(tp_->IsEmpty()){
                tp_->ThreadIdle(); //sleep
            }
            Task t;
            tp_->PopTask(t);
            tp_->UnLockQueue();
            LOG(INFO, "task has be taken, handler....");
            t.Run();
        }
    }

private:
    int thread_total_num; //thread total
    int thread_idle_num; //sleep thread
    std::queue<Task> task_queue;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    volatile bool is_quit;
};

#endif
