How to run:

First you will have to move a copy of the lemur-light directory into the ND-Detection directory. Once thats done replace the "eval_data" directory in lemur-light with the "eval_data" directory we provided. 

then run the following two commands from inside the "ND-Detection" directory:
make
./ND-Detector.sh

The second command may need to be run more than once because our random sampling can result in a dataset with no exact duplicates.

You can also change the number of files to randomly sample by altering the "ND-Detector.sh" script. In the script change the first argument passed to "./RandomSampling.sh".

To clean up the files that are output by our program run "make clean" command.
