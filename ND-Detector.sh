#!/bin/bash

SAMPLESIZE=1000
DATAPATH=lemur-light/eval_data

mkdir data
mkdir data/comments
mkdir data/metadata
	
echo "Trying to generate a random sample of $SAMPLESIZE comments"
./RandomSampling.sh $SAMPLESIZE originalData data
	
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

