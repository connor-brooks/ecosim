CC=gcc
CFLAGS=-g3
LIBS=-lGL -lm -lglut -lglfw -lGLEW

all: ecosim

ecosim: graphics.o utils.o main.o
	$(CC) $(CFLAGS) $(LIBS) graphics.o utils.o main.o -o ecosim

main.o: main.c
	$(CC) -c main.c

utils.o: utils.c
	$(CC) -c utils.c

graphics.o: graphics.c
	$(CC) -c graphics.c

clean:
		rm *.o
		rm ecosim
