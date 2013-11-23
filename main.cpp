#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>

#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "sha1-0.2/sha1.h"

#define SHA1Size 20

using namespace std;

struct file {
  string name;
  string comment;
};

struct cluster {
  file head;
  list<file> files;
};

list<cluster> clusters;
list<file> files;
file f;

typedef pair<unsigned char*,string> SHA1FileNamePair;
list<SHA1FileNamePair> Hash_NameList;

void printSHA1(unsigned char*c){
   
    int l = SHA1Size;
    while(l>0){
        printf("%02x ",*c);
        c++;
        l--;
    }
}


// SHA1FileNamePair Comparison, sort in ascending order
bool compare_SHA1(SHA1FileNamePair first, SHA1FileNamePair second){
    unsigned char* f = first.first;
    unsigned char* s = second.first;

    for(int i=0;i<SHA1Size; i++){
        if(*f<*s)
            return true;
        else if(*f>*s)
            return false;
        f++;
        s++;
    }
}

//deletes the files of any exact duplicate comments
void removeDups() {
  ofstream del, forms;
  forms.open("forms.txt");
  del.open("duplicate_comments.txt");
  for(list<file>::iterator it=files.begin(); it != --files.end(); it++) {
    int numofdups = 0;
    for(list<file>::iterator it2=it; it2 != files.end(); it2++) {
      if(it2 == it) { it2++; }
      if(!strcmp((*it).comment.c_str(), (*it2).comment.c_str())) {
	del << (*it2).name << "\n";
	it2 = files.erase(it2);
	numofdups++;
      }
    }
    if(numofdups > 4) forms << (*it).name << "\n";
  }
  cout << files.size() << endl;
  del.flush();
  del.close();

  forms.flush();
  forms.close();
}

void writeDataFile() {
  string fp = "comment_data.data";
  ofstream f;
  f.open(fp.c_str());

  for(list<file>::iterator it=files.begin(); it!=files.end(); it++) {
    
  }

  f.flush();
  f.close();
}

int preprocess() {
  
  string dir = "data/";
  DIR *dp;
  struct dirent *dirp;
  struct stat filestat;

  dp = opendir("data");
  if(dp == NULL) {
    cout << "failed to open directory" << endl;
    return errno;
  }

  int filenum = 0;
  while((dirp = readdir(dp))) {
    if(!strncmp(dirp->d_name, "..", 2) || !strncmp(dirp->d_name, ".", 1) || !strncmp(dirp->d_name, "metadata", 8) || !strncmp(dirp->d_name, "comments", 8)){
      continue;
    }
    
    string line;
    fstream in;
    ofstream out;

    string filepath =  dir + dirp->d_name;
    in.open(filepath.c_str());

    string newfilepath = "metadata/";
    newfilepath = dir + newfilepath + dirp->d_name;
    out.open(newfilepath.c_str());
    
    //read the metadata
    while(getline(in, line)) {
      if(!strncmp(line.c_str(), "comment =", 9)) { break; }
      out << line << "\n";
    }

    //read the comment and remove line breaks
    string comment;
    comment += line;
    while(getline(in, line)) {
      comment += line;
    }
    string::size_type pos = 0;
    while((pos = comment.find("\r", pos)) != string::npos) {
      comment.replace(pos, 1, " ");
    }
    //remove the "comment =" from the comment
    //comment.erase(0, 10);
    
    ofstream com;
    string path = "comments/";
    path = dir + path + dirp->d_name;
    com.open(path.c_str());
    com << comment;
    com.flush();
    com.close();

    //create entry for file and add to the list
    f.name = dirp->d_name;
    f.comment = comment;
    files.push_back(f);
    
    // Using SHA1 to get the hash value of the comment 
    char* com_tmp = new char[comment.length()+1];
    strcpy(com_tmp, comment.c_str());

    SHA1* sha1 = new SHA1();
    sha1->addBytes(com_tmp,comment.length());
    unsigned char* digest = sha1->getDigest();
    delete sha1;

    // Create pair and add it to Hash_NameList  
    string fileName = dirp->d_name;
    SHA1FileNamePair sfp(digest, fileName);
    Hash_NameList.push_back(sfp);
    
    filenum++;

    in.close();
    out.flush();
    out.close();
  }

  removeDups();
  //writeDataFile();
  //writecommentFiles();

  return 0;
}

int main(int argc, char *argv[]){

  preprocess();

  Hash_NameList.sort(compare_SHA1);
  
  // Debug: Print the Hash_Namelist
  for(list<SHA1FileNamePair>::iterator it=Hash_NameList.begin(); it!=Hash_NameList.end(); it++) {
        printSHA1((*it).first); 
        cout<< " " << (*it).second << endl;
  }


}
