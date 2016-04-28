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
using namespace std;

// Lets go ahead and get that disk declared


class FSYS {
public:
  FSYS();
  void print();
  string toString();
  bool save();
  bool load();
  //Look up Fucking String array overloarding and DO IT
private:
  unsigned short int disk[512];
  // Records will contain all pertinent information to find files
  //  in disk and will be dynamically added to the FAT.
  struct FAT_FUCKING_Record {
    string fileName;
    short int location, size;
  };
  typedef vector<FAT_FUCKING_Record> FAT ;

};
