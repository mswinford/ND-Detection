#!/bin/bash

cd lemur-light/
pwd
gmake

cd eval_data/

# Create dataset and query to utilize lemur api
../app/obj/BuildIndex build_stemmed_nostopw_param dataset.txt
../app/obj/ParseToFile parse_raw_query_stemmed_nostopw_param raw_query.txt
../app/obj/RetrievalEval eval_stemmed_nostopw_param final_query


