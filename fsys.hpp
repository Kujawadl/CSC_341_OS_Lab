/*
 * fsys.hpp
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
#include <unistd.h> // usleep
#include "utils.hpp"
using namespace std;

typedef vector<unsigned short int> FileBuffer;

class FSYS {
public:
  FSYS();
  void print();
  void printDisk(int x);
  int active();
  string toString();
  void load();
  FileBuffer operator[](const string&);
private:
  // Records will contain all pertinent information to find files
  //  in disk and will be dynamically added to the FAT.
  struct FAT_Record {
    string fileName;
    short int location, size;
    FAT_Record() : fileName(""), location(0), size(0) {}
    FAT_Record(string f, short int l, short int s) :
      fileName(f), location(l), size(s) {}
  };
  typedef vector<FAT_Record> FAT;

  unsigned short int disk[512];
  FAT fileTable;
  FileBuffer buffer;

  void loadFAT();
  void loadFile(FAT_Record);

};
