CC=gcc
CFLAGS=-g3
LIBS=-lGL -lm -lglut -lglfw -lGLEW

all: ecosim

ecosim: keyboard.o graphics.o utils.o agents.o main.o
	$(CC) $(CFLAGS) $(LIBS) keyboard.o graphics.o utils.o agents.o main.o -o ecosim

main.o: main.c
	$(CC) -c main.c

agents.o: agents.c
	$(CC) -c agents.c

utils.o: utils.c
	$(CC) -c utils.c

keyboard.o: keyboard.c
	$(CC) -c keyboard.c

graphics.o: graphics.c
	$(CC) -c graphics.c

clean:
		rm *.o
		rm ecosim
