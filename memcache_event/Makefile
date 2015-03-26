CC=g++
CC_FLAG=-g
LIB=-lpthread /usr/local/lib/libevent.so
INCLUDE=memcache_thread.h memcache_event.h

main : main.o memcache_event.o memcache_thread.o $(INCLUDE)
	$(CC) $(CC_FLAG) main.o memcache_event.o memcache_thread.o -o main $(LIB)

main.o : main.cpp $(INCLUDE) 
	$(CC) $(CC_FLAG) -c main.cpp $(LIB)

memcache_event.o : memcache_event.cpp $(INCLUDE) 
	$(CC) $(CC_FLAG) -c memcache_event.cpp $(LIB)

memcache_thread.o : memcache_thread.cpp $(INCLUDE) 
	$(CC) $(CC_FLAG) -c memcache_thread.cpp $(LIB)

clean:
	rm *.o
