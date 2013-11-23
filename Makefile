CC = g++
CFLAGS = -O2 -g

all: main

clean:
	rm main main.o

main:	main.o sha1.o
	$(CC) $(CFLAGS) -o main main.o sha1.o

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp

sha1.o : sha1-0.2/sha1.cpp
	$(CC) -c $(CFLAGS) sha1-0.2/sha1.cpp

