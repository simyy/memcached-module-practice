/*************************************************************************
	> File Name: memcache_slab.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon 09 Feb 2015 07:16:00 AM PST
 ************************************************************************/

#include <iostream>
#include <stdlib.h>

#include "memcache_slab.h"

using namespace std;

MemcacheSlab::MemcacheSlab(const int chunk_size ,const double factor)
{
    int i = POWER_SMALLEST - 1;
    unsigned int size = sizeof(item) + chunk_size;

    cout<<"start init slabs size ..."<<endl;
    while (++i < POWER_LAGEST && size <= ITEM_MAX_SIZE / factor) {
        slabclass[i].size = size; 
        slabclass[i].perslab = ITEM_MAX_SIZE/size;

        size = size * factor;
    }

    int power_largest = i;
    slabclass[power_largest].size = ITEM_MAX_SIZE;
    slabclass[power_largest].perslab = 1;
}

MemcacheSlab::~MemcacheSlab()
{

    cout<<"delete slabs"<<endl;

    int i = POWER_SMALLEST - 1;
    while (++i < POWER_LAGEST) {

    }
}

void MemcacheSlab::slabs_preallocate(const unsigned int maxslabs)
{
}

unsigned int MemcacheSlab::slabclass_id(const size_t size)
{
    int res = POWER_SMALLEST;

    if (size == 0)
        return 0;

    while(size > slabclass[res].size) {
        if (res++ == power_largest)
            return 0;
    }

    cout<<"get slab id:\t"<<res<<endl;

    return res;
}

void MemcacheSlab::do_slabs_alloc(const size_t size, const unsigned int id)
{
    void* ret = NULL;
    item* it  = NULL;
    struct Slab *p = &slabclass[id];

    if (!(p->free_size != 0) || (do_slabs_newslab(id) != 0)) {
        return; // xxxx
    } else {
        /* return off our freelist */
        it = (item *)p->slots;
        p->slots = it->next;
        if (it->next) it->next->prev = 0;
        p->free_size--;
        ret = (void *)it;
    }
    return;
}

int MemcacheSlab::do_slabs_newslab(const unsigned int id)
{
    struct Slab* p = &slabclass[id];
    int len = p->size;

    if (p->slabs == p->list_size) {
        //grow 2*size, or init 16
        size_t new_size =  (p->list_size != 0) ? p->list_size * 2 : 16;
        void* new_list = realloc(p->slab_list, new_size*sizeof(void*));
        if (new_list == 0)
            return 0;
        p->list_size = new_size;
        p->slab_list = new_list;
        cout<<"generate slab success, id:\t"<<id<<", size:\t"<<len<<endl;
    } 

    return 1;
}

void MemcacheSlab::init()
{
    for (int i = 0; i < POWER_LAGEST; i++) {
        do_slabs_newslab(i);
    } 
}
