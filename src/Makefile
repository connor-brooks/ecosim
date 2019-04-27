CC=gcc

all: ecosim

CFLAGS=-g3
LIBS=-lGL -lm -lglfw -lGLEW


ecosim: logger.o quadtree.o graphics.o utils.o agents.o input.o main.o
	$(CC) $(CFLAGS) logger.o quadtree.o graphics.o utils.o agents.o input.o main.o -o ecosim $(LIBS)

main.o: main.c
	$(CC) -c main.c

input.o: input.c
	$(CC) -c input.c

agents.o: agents.c
	$(CC) -c agents.c

utils.o: utils.c
	$(CC) -c utils.c

graphics.o: graphics.c
	$(CC) -c graphics.c

quadtree.o: quadtree.c
	$(CC) -c quadtree.c

logger.o: logger.c
	$(CC) -c logger.c

clean:
	rm *.o
	rm ecosim
