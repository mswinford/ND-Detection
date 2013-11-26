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
#define seed_threshold 5

using namespace std;

int totaldocs;
int docs_after_removal;
int numofSeedDocs;
ofstream stats, fileset, seeds;


struct file {
  string name;
  string comment;
};

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

bool SHA1cmp(SHA1FileNamePair first, SHA1FileNamePair second){
  unsigned char* f = first.first;
  unsigned char* s = second.first;

  for(int i=0;i<SHA1Size; i++){
        if(*f!=*s)
            return false;
        f++;
        s++;
    }

  return true;
}

bool compare_date(string l1, string l2) {
  int m1, d1, y1;
  int m2, d2, y2;

  l1.erase(0, 16);
  l2.erase(0, 16);
  
  string delimiter = "/";

  size_t pos = 0;
  string tok;
  pos = l1.find(delimiter);
  tok = l1.substr(0, pos);
  m1 = atoi(tok.c_str());
  l1.erase(0, pos + delimiter.length());

  pos = l1.find(delimiter);
  tok = l1.substr(0, pos);
  d1 = atoi(tok.c_str());
  l1.erase(0, pos + delimiter.length());

  pos = l1.find(delimiter);
  tok = l1.substr(0, pos);
  y1 = atoi(tok.c_str());
  l1.erase(0, pos + delimiter.length());

  pos = l2.find(delimiter);
  tok = l2.substr(0, pos);
  m2 = atoi(tok.c_str());
  l2.erase(0, pos + delimiter.length());

  pos = l2.find(delimiter);
  tok = l2.substr(0, pos);
  d2 = atoi(tok.c_str());
  l2.erase(0, pos + delimiter.length());

  pos = l2.find(delimiter);
  tok = l2.substr(0, pos);
  y2 = atoi(tok.c_str());
  l2.erase(0, pos + delimiter.length());


  if(y2 < y1) return false;
  if(m2 < m1) return false;
  if(d2 <= m1) return false;
  
  return true;
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

    in.close();
    out.flush();
    out.close();
  }

  return 0;
}

void getSeedDocs() {
  for(list<SHA1FileNamePair>::iterator it=Hash_NameList.begin(); it!=Hash_NameList.end(); it++) {
    list<SHA1FileNamePair> l;
    int numOfDups = 1;
    l.push_back(*it);
    it++;
    while(SHA1cmp(*(l.begin()), *(it))) {
      numOfDups++;
      l.push_back(*it);
      it++;
    }
    list<SHA1FileNamePair>::iterator i = l.begin();
    string seedfile = (*i).second;
    i++;
    while(i!=l.end()) {
      fstream in1, in2;
      string path = "data/metadata/";
      string fp = path + (*i).second;
      string fp2 = path + seedfile;
      in1.open(fp.c_str());
      in2.open(fp2.c_str());
      string line1, line2;
      getline(in1, line1);
      getline(in2, line2);
      if(compare_date(line1, line2)) { seedfile = (*i).second; }
      i++;
    } 
    totaldocs += numOfDups;
    if(numOfDups > seed_threshold){ numofSeedDocs++; seeds << seedfile << "\n"; }
    fileset << seedfile << "\n";
    docs_after_removal++;
    it--;
  }
  stats << "Total number of documents originally = " << totaldocs << "\n";
  stats << "Number of documents after exact duplicate removal = " << docs_after_removal << "\n";
  stats << "Number of seed documents = " << numofSeedDocs << "\n";
}

int main(int argc, char *argv[]){

  preprocess();

  Hash_NameList.sort(compare_SHA1);
  
  // Debug: Print the Hash_Namelist
  //  for(list<SHA1FileNamePair>::iterator it=Hash_NameList.begin(); it!=Hash_NameList.end(); it++) {
  //        printSHA1((*it).first); 
  //       cout<< " " << (*it).second << endl;
  // }

  stats.open("stats.txt");
  fileset.open("fileset.txt");
  seeds.open("seeds.txt");

  getSeedDocs();

  stats.flush();
  stats.close();
  fileset.flush();
  fileset.close();
  seeds.flush();
  seeds.close();

}
