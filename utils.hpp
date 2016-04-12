/*
 * utils.hpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341.001
 * Spring 2016
 */

#ifndef UTILS
#define UTILS

#include <iostream> // cout
#include <iomanip>  // setw, left, right
#include <cstdlib>  // srand, rand
#include <ctime>    // time
#include <iterator> // begin, end
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

string padding(int);
string padding(int, char);
string horizontalrule();
string textboxline(string);
string textbox(string);
string titlebox(string);

#endif
