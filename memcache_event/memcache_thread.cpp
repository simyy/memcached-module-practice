/*************************************************************************
	> File Name: memcache_thread.cpp
	> Author: 
	> Mail: 
	> Created Time: Sat 31 Jan 2015 05:32:53 AM PST
 ************************************************************************/

#include <iostream>
#include <event.h>
#include <string.h>

#include "memcache_thread.h"

using namespace std;

MemcacheThread::MemcacheThread(int n)
{
    nthreads = n;
    index = 0;
}

MemcacheThread::~MemcacheThread()
{
}

struct EVENT_THREAD* MemcacheThread::getEventThread()
{
    struct EVENT_THREAD* et = threads[index];
    if (index >= nthreads-1)
        index = 0;
    else
        index += 1;
    return et; 
}

void MemcacheThread::initThreads()
{
    for (int i = 0; i < nthreads; ++i) {
        cout<<"init threads\t"<<threads.size()<<endl;
        /* create pip for each thread */
        int fds[2];
        if (pipe(fds)) {
            cout<<"pipe error"<<endl;
            return; 
        }

        struct EVENT_THREAD* et = new EVENT_THREAD();
        et->notify_receive_fd = fds[0];
        et->notify_send_fd = fds[1];

        setup_thread(et);
        
        threads.push_back(et); 
    }

    for (int i = 0; i < nthreads; ++i) {
        cout<<"create worker\t"<<i<<endl;
        create_worker(worker_event, threads[i]);
    }
}

void MemcacheThread::setup_thread(struct EVENT_THREAD* et)
{
    et->base = event_init();
    //et->base = event_base_new();
    if (!et->base) {
        cout<<"event_init error"<<endl;
        return ; 
    }

    event_set(&et->notify_event, et->notify_receive_fd,
            EV_READ|EV_PERSIST, thread_process, &index);
    event_base_set(et->base, &et->notify_event);

    if (event_add(&et->notify_event, 0) == -1) {
        cout<<"event_add error"<<endl;   
    }
}

void MemcacheThread::create_worker(void *(*func)(void*), void *arg)
{
    pthread_t thread;
    pthread_attr_t attr;
    int ret;

    pthread_attr_init(&attr);

    ret = pthread_create(&thread, &attr, func, arg);
    if (ret !=0 ) {
        cout<<"pthread_create error"<<endl;
        return ;
    }
}

static void thread_process(int fd, short which, void *arg)
{
    int* x = (int*)arg;
    char buf[1024];
    bzero(buf, sizeof(buf));
    read(fd, buf, sizeof(buf));
    cout<<"Thread["<<*x<<"]\t"<<buf<<endl;
}

static void* worker_event(void *arg)
{
    struct EVENT_THREAD *et = (struct EVENT_THREAD*)arg;
    event_base_loop(et->base, 0);
    return NULL;
}
