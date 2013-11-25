#!/bin/bash

DATAPATH=lemur-light/eval_data

mkdir data
mkdir data/comments
mkdir data/comments
	
echo "Trying to generate a random sample of 100 comments"
./RandomSampling.sh 100 originalData data
	
echo "Trying to generate dataset and a list of seed documents"
./main
java QueryFile seeds.txt raw_query.txt
java QueryFile fileset.txt dataset.txt

echo "Trying to prepare files for executing lemur-light"

cp raw_query.txt $DATAPATH/
cp dataset.txt $DATAPATH/

echo "Trying to generate ND-cluster"
./GenerateNDCluster.sh

echo "Trying to copy retrieval result to current folder"

cp $DATAPATH/result_okapi_stemmed_nostopw ND-Cluster.txt

