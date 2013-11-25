#Author: Zhiyuan Zheng

# Generate a random sample of files from source directory, and put the sample files to destination 

# The script will initially remove txt files in destination for the purpose of project

# check arguments' validity 
if [ $# -lt 3 ]
then 
    echo
    echo "$0 needs three arguments as follow:"
    echo "#of sample files, source, destination"
    exit
fi

if [ ! -d $2 ]
then 
    echo "$0: Directory $2 doesn't exist."
    exit
fi

if [ ! -d $3 ]
then 
    echo "$0: Directory $3 doesn't exist."
    exit
fi

# remove txt files in destination 
rm -f $3/*.txt 

# Randomly generate a list of files
ls $2 | shuf -n $1  > outfile

# Copy each file in the lists to destination
while read line
do
    cp $2/$line $3
done < outfile 

rm outfile



