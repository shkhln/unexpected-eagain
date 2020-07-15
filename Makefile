
.PHONY: all clean test

all: server client

server: main.c
	/compat/linux/usr/bin/cc -DSERVER -std=c99 -Wall -m32 ./main.c -pthread -o $(.TARGET)

client: main.c
	/compat/linux/usr/bin/cc -DCLIENT -std=c99 -Wall -m64 ./main.c -pthread -o $(.TARGET)

clean:
	rm -f server client

test: server client
	./server
