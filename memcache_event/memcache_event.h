/*************************************************************************
	> File Name: memcache_event.h
	> Created Time: Wed 28 Jan 2015 05:38:41 AM PST
 ************************************************************************/

#ifndef _MEMCACHE_EVENT_H
#define _MEMCACHE_EVENT_H

#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "memcache_thread.h"
    
static void do_accept(evutil_socket_t sfd, short event, void *arg);

class MemcacheEvent
{
public:
    MemcacheEvent(int n);
    ~MemcacheEvent();

    void run(); 

protected:

private:
    int    port;
    evutil_socket_t sfd;
    struct sockaddr_in sin;
    struct event_base* main_base;
    struct event *listener_event;  

    struct MemcacheThread* memcache_thread;
    
    int    nthreads;
};

#endif
