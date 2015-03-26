/*************************************************************************
	> File Name: memcache_thread.h
	> Author: 
	> Mail: 
	> Created Time: Sat 31 Jan 2015 05:31:38 AM PST
 ************************************************************************/

#ifndef _MEMCACHE_THREAD_H
#define _MEMCACHE_THREAD_H

#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <event2/event.h>
#include <event2/event_struct.h>

using namespace std;

/* event call back function */
static void thread_process(int fd, short which, void *arg);
/* worker thread:main event loop */
static void* worker_event(void *arg);

struct EVENT_THREAD{
    pthread_t thread_id;        /* unique ID of this thread */
    struct event_base *base;    /* libevent handle this thread uses */
    struct event notify_event;  /* listen event for notify pipe */
    int notify_receive_fd;      /* receiving end of notify pipe */
    int notify_send_fd;         /* sending end of notify pipe */
};

class MemcacheThread
{
public:
    MemcacheThread(int);
    ~MemcacheThread();

    void initThreads();

    struct EVENT_THREAD* getEventThread();

protected:

    void setup_thread(struct EVENT_THREAD* et);
    void create_worker(void *(*func)(void*), void *arg);

private:
    vector<struct EVENT_THREAD*> threads;
    int nthreads;
    int index;
};

#endif
