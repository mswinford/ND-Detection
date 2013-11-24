This file contains information on how the programs works.

To run everything you first have to put lemur-light into the directory and configure it. This is left out of our submission because the size is to big. Ince the lemur toolkit is set up, simply type "make" into the command line.

main.cpp: (Zhiyuan Zheng and Michael Swinford)
This program starts by separating the comments and there metadata into separate files. Then it hashes each comment and sorts them to make it faster to find duplicates. After that is done it generates three files: "stats.txt", "fileset.txt", and "seeds.txt".
 
"stats.txt" stores three bits of information: the number of files originally, the number of files after exact duplicates are removed, and the number of files with more than five exact duplicates.

"fileset.txt" stores a list of all the files with the exact duplicates removed. For exact duplicates, we keep the file with the earliest date in its metadata.

"seeds.txt" stores a list of the seed documents that had five or more exact duplicates. The threshold for the number of duplicates can be changed.

QueryFile.java: (Zhe Cao)
This program takes in the names of an input file and an output file. The input file contains a list of files that should be used to create a document that can be passed into the lemur toolkit. The output file is simply the name the user wants for the output file. This program is run twice: once to create a document with all of the files that we will be querying, and another time to create a file containing the seed documents which we will use to make queries.

After we have these two docs we use lemur to parse queries and use the resulting queries to group the documents by Okapi score.
