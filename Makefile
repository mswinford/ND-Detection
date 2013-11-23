CC = g++
CFLAGS = -O2 -g

all: main

clean:
	rm main main.o

main:	main.o
	$(CC) $(CFLAGS) -o main main.o

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp


