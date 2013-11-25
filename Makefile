CC = g++
JAVA=javac
CFLAGS = -O2 -g

all: main

clean:
	rm main *.o *.class

main:	main.o sha1.o QueryFile.java
	$(CC) $(CFLAGS) -o main main.o sha1.o	
	$(JAVA) QueryFile.java

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp

sha1.o : sha1-0.2/sha1.cpp
	$(CC) -c $(CFLAGS) sha1-0.2/sha1.cpp


