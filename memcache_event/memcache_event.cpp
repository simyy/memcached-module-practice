/*************************************************************************
	> File Name: memcache_event.cpp
	> Created Time: Wed 28 Jan 2015 05:43:57 AM PST
 ************************************************************************/

#include <iostream>

#include "memcache_event.h"

using namespace std;

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct MemcacheThread* memcache_thread = (struct MemcacheThread*)arg; 
    struct sockaddr ss; 
    socklen_t slen = sizeof(ss);  

    int fd = accept(listener, (struct sockaddr*)&ss, &slen);  
    if (fd < 0)  
    {  
        perror("accept");  
    }  
    else if (fd > FD_SETSIZE)  
    {  
        close(fd);  
    }  
    else
    {
        cout<<"accept something"<<endl;
        char buf[1024];

        bzero(buf, sizeof(buf));
        int n = recv(fd, buf, sizeof(buf), 0);
        if (n > 0)
        {
            cout<<"recv:\t"<<buf<<endl;
        }

        struct EVENT_THREAD*  notify_thread = memcache_thread->getEventThread();
        int flag;
        flag = write(notify_thread->notify_send_fd, "hello", sizeof("hello"));
        if (!flag) {
            cout<<"write error"<<endl;
        }
    }
}

MemcacheEvent::MemcacheEvent(int n)
{
    nthreads = n;
    
    main_base = event_base_new();

    port = 6666;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(sfd);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0);
    sin.sin_port = htons(port);

    memcache_thread = new MemcacheThread(nthreads);
}

MemcacheEvent::~MemcacheEvent()
{
    delete memcache_thread;
}

void MemcacheEvent::run()
{
    int flag = 1;
    if( setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == 1) 
    { 
        perror("setsockopt"); 
        exit(1); 
    } 

    flag = bind(sfd, (struct sockaddr*)&sin, sizeof(sin));
    if (flag < 0)
    {
        cout<<"bind"<<endl;
        return;
    }

    flag = listen(sfd, 25);
    if (flag < 0)
    {
        cout<<"listen"<<endl;
        return;
    }

    listener_event = event_new(main_base, sfd, EV_READ|EV_PERSIST, do_accept, (void*)memcache_thread);
    event_add(listener_event, NULL);

    memcache_thread->initThreads();
    
    event_base_dispatch(main_base);
}
