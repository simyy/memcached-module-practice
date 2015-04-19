/*************************************************************************
	> File Name: memcache_slab.h
	> Author: 
	> Mail: 
	> Created Time: Mon 09 Feb 2015 06:04:02 AM PST
 ************************************************************************/

#ifndef _MEMCACHE_SLAB_H
#define _MEMCACHE_SLAB_H

//size of slabclass
#define POWER_LAGEST 10
#define POWER_SMALLEST 1

//the max size of item is 2M, and other's size is 2M/(factor^n)
#define ITEM_MAX_SIZE 2048 

#define MAX_NUMBER_OF_SLAB_CLASSES (POWER_LAGEST+1)

static const int mem_limit = 512*1024*1024;

struct Slab{
   unsigned int size;      //size of a item
   unsigned int perslab;   //how many items per slab

   void*  slots;           //list of item ptrs
   unsigned int slabs;     //how many slabs allocated
   unsigned int free_size; //total free items in list

   void **slab_list;       //array of slab pointers
   unsigned int list_size; //size of prev array

   unsigned int sl_curr; //total free items in list
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
    MemcacheSlab(const int chunk_size, const double factor);
    ~MemcacheSlab();

    /* init slab chunk size and slab factor */
    void init(int prealloc=0);

    /* alloc a item  */
    void do_item_alloc(char* key, char* data);
protected:
    /* enlarge the size of slab */ 
    int grow_slab_list(const unsigned int id);

    void slabs_preallocate(const unsigned int maxslabs);

    int do_slabs_newslab(const unsigned int id);

    /* alloc a size item */
    void do_slabs_alloc(const size_t size, const unsigned int id);

    /* get the id of slabclass, it's size fit */
    unsigned int slabclass_id(const size_t size);

    /* split the memory of ptr in a slab->slab_list for use in future */
    void split_slab_page_into_freelist(char *ptr, const unsigned int id);

    /* get the size of memory from ptr, and add it to the list of slab[id] */
    void do_slabs_free(void *ptr, const size_t size, unsigned int id);

    /* */
    void* memory_allocate(size_t size);

private:
    int power_largest;

    int m_chunk_size;
    int m_factor;

    void* mem_base;

    struct Slab slabclass[POWER_LAGEST+1];

};

#endif
