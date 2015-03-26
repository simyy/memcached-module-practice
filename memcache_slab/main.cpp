/*************************************************************************
	> File Name: main.cpp
	> Created Time: Wed 28 Jan 2015 05:33:26 AM PST
 ************************************************************************/

#include <iostream>

#include "memcache_slab.h"

using namespace std;

int main(int argc, char** argv)
{
    MemcacheSlab* ms = new MemcacheSlab(1024, 1.25);
    ms->init();

    delete  ms;

}
