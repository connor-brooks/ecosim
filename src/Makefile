CC=gcc

all: ecosim

CFLAGS=-g3
LIBS=-lGL -lm -lglfw -lGLEW


ecosim: quadtree.o graphics.o utils.o agents.o main.o
	$(CC) $(CFLAGS) $(LIBS) quadtree.o graphics.o utils.o agents.o main.o -o ecosim

main.o: main.c
	$(CC) -c main.c

agents.o: agents.c
	$(CC) -c agents.c

utils.o: utils.c
	$(CC) -c utils.c

graphics.o: graphics.c
	$(CC) -c graphics.c

quadtree.o: quadtree.c
	$(CC) -c quadtree.c

clean:
	rm *.o
	rm ecosim
