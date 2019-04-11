CC=gcc

all: ecosim

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS=-g3
	LIBS=-lGL -lm -lglfw -lGLEW
endif

ifeq ($(UNAME_S),Darwin)
	CFLAGS=-g3
	LIBS=-framework OpenGl -I/usr/local/include -lglfw3 -lm --lGLEW
endif

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
