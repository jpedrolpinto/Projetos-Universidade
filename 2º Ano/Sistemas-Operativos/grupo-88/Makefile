CC = gcc
CFLAGS = -Wall -g -std=c99
LDFLAGS = 

all: dserver dclient

dserver: src/dserver.o
	$(CC) $(LDFLAGS) $^ -o $@

dclient: src/dclient.o
	$(CC) $(LDFLAGS) $^ -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o dserver dclient