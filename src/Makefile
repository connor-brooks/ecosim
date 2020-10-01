CC=gcc

all: ecosim

CFLAGS=-g3 -pedantic -Wall -Wextra
LIBS=-lGL -lm -lglfw -lGLEW


ecosim: logger.o quadtree.o graphics.o utils.o agents.o input.o main.o
	$(CC) $(CFLAGS) logger.o quadtree.o graphics.o utils.o agents.o input.o main.o -o ecosim $(LIBS)

main.o: main.c
	$(CC) ${CFLAGS} -c main.c

input.o: input.c
	$(CC) ${CFLAGS} -c input.c

agents.o: agents.c
	$(CC) ${CFLAGS} -c agents.c

utils.o: utils.c
	$(CC) ${CFLAGS} -c utils.c

graphics.o: graphics.c
	$(CC) ${CFLAGS} -c graphics.c

quadtree.o: quadtree.c
	$(CC) ${CFLAGS} -c quadtree.c

logger.o: logger.c
	$(CC) ${CFLAGS} -c logger.c

clean:
	rm *.o
	rm ecosim
