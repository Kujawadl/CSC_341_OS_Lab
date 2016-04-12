/*
 * utils.hpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341.001
 * Spring 2016
 */

#ifndef UTILS
#define UTILS

#include <iostream> // cout, cerr
#include <iomanip>  // setw, setfill, left, right, dec, endl
#include <cstdlib>  // srand, rand
#include <ctime>    // time
#include <sstream>  // stringstream
#include <stdexcept>// out_of_range
  using namespace std;

// 64 4-word frames = 256 words of memory
#define NUM_FRAMES 64

// Case-insensitive string comparison
bool my_strcasecmp(string str1, string str2);

// Convert int to string
string itos(int);

// Bit array containing information about the frames in memory
typedef bool FrameTable[NUM_FRAMES];

class PageTable
{
  private:
    // Refers to the Operating System's FrameTable; thus, if this
    // PageTable changes a frame's in-use status, all other PageTables
    // will see the change.
    FrameTable& _framesInUse;
    // Unique to this PageTable; contains the actual page-frame mappings
    int _pageTable[NUM_FRAMES];

    // Iterators not fully supported in C++98; this assists with std::find()
    struct ary {
      static int* begin(int (&arr)[64]) {return arr;}
      static int* end(int (&arr)[64]) {return begin(arr) + NUM_FRAMES;}
      static int* begin(FrameTable& arr) {
        return static_cast<int*>(static_cast<void*>(arr));}
      static int* end(FrameTable& arr) {return begin(arr) + NUM_FRAMES;}
    };

  public:
    PageTable(FrameTable&);
    int& operator[] (const int);
    string toString();
    void print();
};

// Return a string consisting of n of the specified character (default: space)
string padding(int);
string padding(int, char);

// Return a horizontal rule of # characters (std console width = 80chars)
string horizontalrule();

// Divide the string into 80 character lines with textbox borders left and right
string textboxline(string);

// Return a textbox of width = 80chars, wrapped with # symbols
string textbox(string);

// Return a thicker textbox (i.e.: extra horizontalrule above and below)
string titlebox(string);

#endif
