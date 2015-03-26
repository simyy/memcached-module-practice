/*************************************************************************
	> File Name: main.cpp
	> Created Time: Wed 28 Jan 2015 05:33:26 AM PST
 ************************************************************************/

#include <iostream>

#include "memcache_event.h"
#include "memcache_thread.h"

using namespace std;

int main(int argc, char** argv)
{
    MemcacheEvent* me = new MemcacheEvent(5);
    me->run();

    delete  me;

}
