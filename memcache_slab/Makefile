CC=g++
CC_FLAG=-g
LIB=
INCLUDE=memcache_slab.h

main : main.o memcache_slab.o $(INCLUDE)
	$(CC) $(CC_FLAG) main.o memcache_slab.o -o main $(LIB)

main.o : main.cpp $(INCLUDE) 
	$(CC) $(CC_FLAG) -c main.cpp $(LIB)

memcache_slab.o : memcache_slab.cpp $(INCLUDE) 
	$(CC) $(CC_FLAG) -c memcache_slab.cpp $(LIB)

clean:
	rm *.o
