/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

/*! \page  Retrieval Evaluation Application within light Lemur toolkit

This application runs retrieval experiments to evaluate different retrieval models 

Usage: RetrievalEval parameter_file

Please refor to the namespace LocalParameter for setting the parameters within the parameter_file

 */


#include "common_headers.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include "Param.hpp"
#include "String.hpp"
#include "IndexedReal.hpp"
#include "ScoreAccumulator.hpp"
#include "ResultFile.hpp"
#include "TextQueryRep.hpp"
#include <math.h>   
#include <Index.hpp>



using namespace lemur::api;

double** docsArrs;
int numberOfTermsInQuery; 

namespace LocalParameter{
  std::string databaseIndex; // the index of the documents
  std::string queryStream;   // the file of query stream
  std::string resultFile;    // the name of the result file
  std::string weightScheme;  // the weighting scheme
  int resultCount;           // the number of top ranked documents to return for each query
  double threshold;
  
  void get() {
    // the string with quotes are the actual variable names to use for specifying the parameters
    databaseIndex    = ParamGetString("index"); 
    queryStream      = ParamGetString("query");
    resultFile       = ParamGetString("result","res");
    weightScheme     = ParamGetString("weightScheme","RawTF");
    resultCount      = ParamGetInt("resultCount", 100); 
    threshold        = ParamGetDouble("threshold",0);
  }    
};

void GetAppParam() 
{
  LocalParameter::get();
}

void ComputeQryArr(Document *qryDoc, double *qryArr, Index *ind){
  //compute the array representation of query; it is the frequency representation for the original query 
  for (int t=1; t<=ind->termCountUnique(); t++) {
    qryArr[t]=0;
  }
  
  qryDoc->startTermIteration();
  while (qryDoc->hasMore()) {
    const Term *qryTerm = qryDoc->nextTerm();
    int qryTermID = ind->term(qryTerm->spelling());
    qryArr[qryTermID] ++;
  }
}

// Compute the vector representation of each document in dataset
void ComputeDocArrs(Index *ind){
    // Compute the array representations of all documents in index;
    
    docsArrs = new double*[ind->docCount()+1];
    for(int i=1; i<=ind->docCount();i++){
        docsArrs[i] = new double[ind->termCountUnique()+1];

        for (int t=1; t<=ind->termCountUnique(); t++){
            docsArrs[i][t]=0;
        }
    }
    
    
    for (int docID = 1; docID <= ind->docCount(); docID++) {
       // The function call document(docID) returns the string form of the ID.

      
       // now fetch term info list for each document, this creates an
       // instance of TermInfoList, which needs to be deleted later! 
       TermInfoList *termList = ind->termInfoList(docID);
       
       // iterate over entries in termList, i.e., all words in the document
       termList->startIteration();   
       TermInfo *tEntry;
       while (termList->hasMore()) {
          tEntry = termList->nextEntry(); 
         
          docsArrs[docID][tEntry->termID()] = tEntry->count();
       }
       delete termList; // don't forget to delete termList!
    }

}


// Return the cosine similarity value of arrays a and b.
double CosSimilarity(double *aArr, double *bArr, Index *ind){
    
    double sum = 0;
    double aMag = 0; 
    double bMag = 0;

    for(int i=1; i<=ind->termCountUnique();i++){
        sum = sum + aArr[i]*bArr[i];
        aMag = aMag + aArr[i]*aArr[i];
        bMag = bMag + bArr[i]*bArr[i];

    }
    aMag = sqrt(aMag);
    bMag = sqrt(bMag);
    
    return sum / aMag / bMag;
   
}

void Retrieval(double *qryArr, IndexedRealVector &results, Index *ind){
  //retrieve documents with respect to the array representation of the query

    lemur::retrieval::ArrayAccumulator scoreAccumulator(ind->docCount());

  //  scoreAaccumulator.reset();

    for(int d = 1; d<=ind->docCount();d++){
        
        double wt = CosSimilarity(qryArr, docsArrs[d], ind);
        // scoreAccumulator.incScore(d,wt);

        results.PushValue(d,wt);
    }

}


/// A retrieval evaluation program
int AppMain(int argc, char *argv[]) {
  

  //Step 1: Open the index file
  Index  *ind;

  try {
    ind  = IndexManager::openIndex(LocalParameter::databaseIndex);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("RelEval", "Can't open index, check parameter index");
  }

  //Step 2: Open the query file
  DocStream *qryStream;
  try {
    qryStream = new lemur::parse::BasicDocStream(LocalParameter::queryStream);
  } 
  catch (Exception &ex) {
    ex.writeMessage(cerr);
    throw Exception("RetEval", 
                    "Can't open query file, check parameter textQuery");
  }

  //Step 3: Create the result file
  ofstream result(LocalParameter::resultFile.c_str());
  ResultFile resultFile(1);
  resultFile.openForWrite(result, *ind);


  // go through each query
  
  qryStream->startDocIteration();
  while (qryStream->hasMore()) {
    Document *qryDoc = qryStream->nextDoc();
    const char *queryID = qryDoc->getID();
    cout << "query: "<< queryID <<endl;

    double *queryArr = new double[ind->termCountUnique()+1];  //the array that contains the weights of query terms; for orignial query 
    ComputeQryArr(qryDoc,queryArr, ind); 
    
    ComputeDocArrs(ind); 

    IndexedRealVector results(ind->docCount());
    results.clear();

    Retrieval(queryArr,results,ind);

    results.Sort();
    resultFile.writeResults(queryID, &results, LocalParameter::threshold);

    delete queryArr;
  }



  result.close();
  delete qryStream;
  delete ind;
  return 0;
}

