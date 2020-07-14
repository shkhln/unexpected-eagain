
.PHONY: all clean test

all: server client

server: main.c server_seq.c
	/compat/linux/usr/bin/cc -DSERVER -std=c99 -Wall -m32 ./main.c -pthread -o $(.TARGET)

client: main.c client_seq.c
	/compat/linux/usr/bin/cc -DCLIENT -std=c99 -Wall -m64 ./main.c -pthread -o $(.TARGET)

server_seq.c:
	./servergen.rb > $(.TARGET)

client_seq.c:
	./clientgen.rb > $(.TARGET)

clean:
	rm server client server_seq.c client_seq.c

test: server client
	./server
