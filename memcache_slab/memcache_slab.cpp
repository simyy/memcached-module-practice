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
        delete mem_base;
    cout<<"delete slabs"<<endl;
}

void MemcacheSlab::init(int prealloc)
{
    int i = POWER_SMALLEST - 1;
    unsigned int size = sizeof(struct item) + m_chunk_size;

    cout<<"start init slabs ..."<<endl;
    while (++i < POWER_LAGEST && size <= ITEM_MAX_SIZE / m_factor) {
        struct Slab* slab = new Slab; 
        slab->size = size; 
        slab->perslab = ITEM_MAX_SIZE/size;
        slab->slabs = 0;
        slab->sl_curr = 0;
        slabclass.push_back(slab);
        size = size * m_factor;
    }

    struct Slab* slab = new Slab; 
    slab->size = ITEM_MAX_SIZE;
    slab->perslab = 1;
    slab->slabs = 0;
    slab->sl_curr = 0;
    slabclass.push_back(slab);

    if (prealloc) {
        mem_base = new char[mem_limit];
        if (mem_base != NULL) {
            cout<<"prealloc success ..."<<endl;
            memset(mem_base, 0, mem_limit);
        }
        else
            cout<<"prealloc failure ..."<<endl;
    }
}

int MemcacheSlab::do_item_alloc(char* key, char* data)
{

    int len = sizeof(data) + sizeof(key);
    int id = slabclass_id(len);
    struct Slab* p = slabclass[id];

    struct item* m = (struct item*)do_slabs_alloc(len, id);
    memcpy(m->key, key, sizeof(key));
    memcpy(m->data, data, sizeof(data));
    m->next = (struct item*)p->slots;
    ((struct item*)p->slots)->prev = m;
    p->slots = m;
    (p->sl_curr)--;

    return 1;
}

void* MemcacheSlab::do_slabs_alloc(const size_t size, const unsigned int id)
{
    void* ret = NULL;
    item* it  = NULL;
    struct Slab *p = slabclass[id];

    if ((p->sl_curr == 0) && (do_slabs_newslab(id) == 0)) {
        ret = NULL; 
    }
    if (p->sl_curr != 0) {
        /* return off our freelist */
        it = (item *)p->slots;
        p->slots = it->next;
        if (it->next) it->next->prev = 0;
        p->sl_curr--;
        ret = (void *)it;
    }
    return ret;
}

int MemcacheSlab::do_slabs_newslab(const unsigned int id)
{
    struct Slab *p = slabclass[id];
    int len = p->size * p->perslab;
    char *ptr = NULL;

    //grow_slab_list(id);

    ptr = new char(len);
    memset(ptr, 0, len);

    split_slab_page_into_freelist(ptr, id);
    (p->slab_list)[p->slabs++] = (void*)ptr;

    return 1;
}

void MemcacheSlab::split_slab_page_into_freelist(char *ptr, const unsigned int id)
{
    struct Slab *p = slabclass[id];
    for (int i = 0; i < p->perslab; i++) {
        do_slabs_free(ptr, 0, id);
        ptr += p->size;
    }
}

void MemcacheSlab::do_slabs_free(void *ptr, const size_t size, unsigned int id)
{
    if (id < POWER_SMALLEST || id > POWER_LAGEST)
        return;

    struct Slab *p = slabclass[id];

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

int MemcacheSlab::grow_slab_list(const unsigned int id)
{
    struct Slab* p = slabclass[id];
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

unsigned int MemcacheSlab::slabclass_id(const size_t size)
{
    int res = POWER_SMALLEST;

    if (size == 0)
        return 0;

    while(size > slabclass[res]->size) {
        if (res++ == power_largest)
            return 0;
    }

    cout<<"get slab id:\t"<<res<<endl;

    return res;
}
