CC = g++
CFLAGS = -O2 -g

all: main

clean:

main:
	$(CC) $(CFLAGS) -o main main.o

main.o:
	$(CC) -c $(CFLAGS) main.cpp


