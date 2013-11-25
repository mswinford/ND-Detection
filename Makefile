CC = g++
JAVA=javac
CFLAGS = -O2 -g
DATAPATH=lemur-light/eval_data

all: main

clean:
	rm main *.o *.class

run: RandomSampling.sh
	@echo "Trying to generate a random sample of 100 comments"
	@./RandomSampling.sh 100 originalData data
	
	@echo "Trying to generate dataset and a list of seed documents"
	@./main
	@java QueryFile seeds.txt raw_query.txt
	@java QueryFile fileset.txt dataset.txt
	
	@echo "Trying to prepare files for executing lemur-light"
	@cp raw_query.txt $(DATAPATH)/
	@cp dataset.txt $(DATAPATH)/

	@echo "Trying to generate ND-cluster"
	@./GenerateNDCluster.sh

	@echo "Trying to copy retrieval result to current folder"
	@cp $(DATAPATH)/result_okapi_stemmed_nostopw ND-Cluster.txt

main:	main.o sha1.o QueryFile.java
	$(CC) $(CFLAGS) -o main main.o sha1.o	
	$(JAVA) QueryFile.java

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp

sha1.o : sha1-0.2/sha1.cpp
	$(CC) -c $(CFLAGS) sha1-0.2/sha1.cpp


