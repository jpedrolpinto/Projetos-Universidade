CC = gcc 
CFLAGS=-Wall -Wextra -pedantic -O2
LIBS=-lm -lcurses

jogo: ProjetoLI2.c 
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	 rm -f *.o jogo