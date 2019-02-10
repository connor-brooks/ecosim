CC=gcc
CFLAGS=-g3
LIBS=-lGL -lm -lglfw

all: ecosim

ecosim: main.o
	$(CC) $(CFLAGS) $(LIBS) main.o -o ecosim

main.o: main.c
	$(CC) -c main.c

clean:
		rm *.o
		rm ecosim
