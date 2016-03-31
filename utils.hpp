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

#define MMU_DEBUG

// 64 4-word frames = 256 words of memory
#define NUM_FRAMES 64

// Case-insensitive string comparison
bool my_strcasecmp(std::string str1, std::string str2);

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

  public:
    PageTable(FrameTable&);
    int& operator[] (const int);
    void print();
};

#endif
