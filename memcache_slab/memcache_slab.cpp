/*************************************************************************
	> File Name: memcache_slab.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon 09 Feb 2015 07:16:00 AM PST
 ************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <linux/string.h> 

#include "memcache_slab.h"

using namespace std;

MemcacheSlab::MemcacheSlab(const int chunk_size ,const double factor)
{
    m_chunk_size = chunk_size;
    m_factor = factor;

    mem_base = NULL;
}

MemcacheSlab::~MemcacheSlab()
{
    if (mem_base != NULL)
        free(mem_base);
    cout<<"delete slabs"<<endl;
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

int MemcacheSlab::do_slabs_newslab(const unsigned int id)
{
    struct Slab *p = &slabclass[id];
    int len = p->size * p->perslab;
    char *ptr = NULL;

    grow_slab_list(id);

    ptr = (char*)memory_allocate((size_t)len);
    memset(ptr, 0, (size_t)len);

    split_slab_page_into_freelist(ptr, id);
    p->slab_list[p->slabs++] = (void*)ptr;

    return 1;
}

void* MemcacheSlab::memory_allocate(size_t size)
{
    void *ret = NULL;

    ret = malloc(size);

    // if prealloc, must guarantee aligned!!!

    return ret;
}


void MemcacheSlab::split_slab_page_into_freelist(char *ptr, const unsigned int id)
{
    struct Slab *p = &slabclass[id];
    for (int i = 0; i < p->perslab; i++) {
        do_slabs_free(ptr, 0, id);
        ptr += p->size;
    }
}

void MemcacheSlab::do_slabs_free(void *ptr, const size_t size, unsigned int id)
{
    if (id < POWER_SMALLEST || id > POWER_LAGEST)\
        return;

    struct Slab *p = &slabclass[id];

    //maybe judge memmory limit there

    //insert ptr in the header of list
    item* it = (struct item*)ptr;
    it->prev = 0;
    it->next = (item*)p->slots;
    if (it->next)
        it->next->prev = it;
    p->slots = it;

    p->sl_curr += 1;
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

int MemcacheSlab::grow_slab_list(const unsigned int id)
{
    struct Slab* p = &slabclass[id];
    int len = p->size * p->perslab;

    if (p->slabs == p->list_size) {
        //grow 2*size, or init 16
        size_t new_size =  (p->list_size != 0) ? p->list_size * 2 : 16;
        void* new_list = realloc(p->slab_list, new_size*sizeof(void*));
        if (new_list == 0)
            return 0;
        p->list_size = new_size;
        p->slab_list = &new_list;
        cout<<"generate slab success, id:\t"<<id<<", size:\t"<<len<<endl;
    } 

    return 1;
}

void MemcacheSlab::init(int prealloc)
{
    int i = POWER_SMALLEST - 1;
    unsigned int size = sizeof(item) + m_chunk_size;

    cout<<"start init slabs size ..."<<endl;
    while (++i < POWER_LAGEST && size <= ITEM_MAX_SIZE / m_factor) {
        slabclass[i].size = size; 
        slabclass[i].perslab = ITEM_MAX_SIZE/size;

        size = size * m_factor;
    }

    int power_largest = i;
    slabclass[power_largest].size = ITEM_MAX_SIZE;
    slabclass[power_largest].perslab = 1;

    if (prealloc) {
        mem_base = malloc(mem_limit);
        if (mem_base != NULL) {
            cout<<"prealloc success ..."<<endl;
            memset(mem_base, 0, mem_limit);
        }
        else
            cout<<"prealloc failure ..."<<endl;
    }
}
