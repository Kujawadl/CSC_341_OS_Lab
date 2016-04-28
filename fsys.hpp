/*
 * fysy.hpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341.001
 * Spring 2016
 */

#include <iostream> // cout, cerr
#include <iomanip>  // setw, setfill, left, right, dec, endl
#include <sstream>  // stringstream
#include <vector>   // we need vectors
#include <fstream>
#include <string>
#include "utils.hpp"
using namespace std;

struct FileBuffer {
  vector<unsigned short int> *buffer;

  FileBuffer() {buffer = new vector<unsigned short int>;}
  int size() {return (*buffer).size();}
  unsigned short int& operator[](int& i) {return (*buffer)[i];}
  void push_back(unsigned short int i) {(*buffer).push_back(i);}
};

class FSYS {
public:
  FSYS();
  void print();
  string toString();
  bool save();
  bool load();
  FileBuffer operator[](const string&);
private:
  unsigned short int disk[512];
  // Records will contain all pertinent information to find files
  //  in disk and will be dynamically added to the FAT.
  struct FAT_Record {
    string fileName;
    short int location, size;
  };
  typedef vector<FAT_Record> FAT;

  FAT fileTable;

};
