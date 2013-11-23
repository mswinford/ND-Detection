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

}
