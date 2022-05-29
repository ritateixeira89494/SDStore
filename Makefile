CFLAGS = -Wall -O0 -g

all: server client

server: bin/sdstored 

client: bin/sdstore

bin/sdstored : obj/sdstored.o
	gcc -o "$@" $^ $(CFLAGS)

obj/sdstored.o: src/sdstored.c
	gcc -c $< -o "$@" $(CFLAGS)

bin/sdstore : obj/sdstore.o
	gcc -o "$@" $^ $(CFLAGS)

obj/sdstore.o: src/sdstore.c
	gcc -c $< -o "$@" $(CFLAGS)



clean:
	rm -f obj/* bin/{sdstore,sdstored}