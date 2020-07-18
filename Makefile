
.PHONY: all clean test

all: server client

server: eagain-bug.c
	/compat/linux/usr/bin/cc -DSERVER -std=c99 -Wall -m32 ./eagain-bug.c -pthread -o $(.TARGET)

client: eagain-bug.c
	/compat/linux/usr/bin/cc -DCLIENT -std=c99 -Wall -m64 ./eagain-bug.c -pthread -o $(.TARGET)

clean:
	rm -f server client

test: server client
	./server
