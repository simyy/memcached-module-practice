/*************************************************************************
	> File Name: memcache_slab.h
	> Author: 
	> Mail: 
	> Created Time: Mon 09 Feb 2015 06:04:02 AM PST
 ************************************************************************/

#ifndef _MEMCACHE_SLAB_H
#define _MEMCACHE_SLAB_H

#define POWER_LAGEST 200
#define POWER_SMALLEST 1
#define ITEM_MAX_SIZE 2048 
#define MAX_NUMBER_OF_SLAB_CLASSES (POWER_LAGEST+1)

struct Slab{
   unsigned int size;      //size of a item
   unsigned int perslab;   //how many items per slab

   void*  slots;           //list of item ptrs
   unsigned int slabs;     //how many slabs allocated
   unsigned int free_size; //total free items in list

   void *slab_list;       //array of slab pointers
   unsigned int list_size; //size of prev array
};

struct item {
    struct item* next;
    struct item* prev;
    struct item* h_next;

    //rel_time_t time;        //least rencent access
    //rel_time_t exptime;     //expire time

    char* key;

    char* data;
};

class MemcacheSlab
{
public:
    //MemcacheSlab(const size_t limit, const double factor);
    MemcacheSlab(const int limit, const double factor);
    ~MemcacheSlab();

    void init();

protected:
    int do_slabs_newslab(const unsigned int id);

    void slabs_preallocate(const unsigned int maxslabs);

    /* alloc a size item */
    void do_slabs_alloc(const size_t size, const unsigned int id);

    /* get the id of slabclass, it's size fit */
    unsigned int slabclass_id(const size_t size);

private:
    int power_largest;

    struct Slab slabclass[POWER_LAGEST];

};

#endif
