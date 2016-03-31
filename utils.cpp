/*
 * utils.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341.001
 * Spring 2016
 */

#include "utils.hpp"

// Case-insensitive string comparison
bool my_strcasecmp(std::string str1, std::string str2)
{
	int length1 = str1.length();

	for (int i = 0; i < length1; i++) {
		char &c = str1.at(i);
		c = tolower(c);
	}
	int length2 = str2.length();
	for (int i = 0; i < length2; i++) {
		char &c = str2.at(i);
		c = tolower(c);
	}
	return ((str1 == str2) ? true : false);
}

// Constructor; takes a FrameTable by reference
PageTable::PageTable(FrameTable& F) : _framesInUse(F)
{
  // Initialize page table with -1 (i.e. no associated frame)
  for (int i = 0; i < NUM_FRAMES; i++) {
    _pageTable[i] = -1;
  }
}

/*
  Subscript operator overload
  If the specified page number (index) has an allocated frame,
  return that frame number. Otherwise, randomly search for an unallocated
  frame, assign said free frame to the specified page number entry,
  and set the FrameTable entry to indicate the frame is now allocated.
*/
int& PageTable::operator[] (const int index)
{
  // If index out of bounds, throw an exception
  if (index > 63 || index < 0) {
    throw std::out_of_range("PageTable index " + std::to_string(index) +
                            " is invalid! Index must be 0-63!\n");
  }
  // If page table contains no frame for specified page
  if (_pageTable[index] < 0) {
    #ifdef MMU_DEBUG
    std::cout << "Page #" << index << " has no associated frame." << std::endl;
    std::cout << "Searching for next available frame:" << std::endl;
    #endif

    // If no more available frames to allocate, throw an exception
    if (std::find(std::begin(_framesInUse), std::end(_framesInUse), false)
        == std::end(_framesInUse)) {
      throw std::out_of_range("No more available frames!");
    }

    // Seed random number generator
    srand(time(NULL));

    // Find the next available frame number
    int i = rand() % 64;
    while (_framesInUse[i] == true) {
      #ifdef MMU_DEBUG
      std::cout << "\tFrame #" << i << " is in use..." << std::endl;
      #endif
      i = rand() % 64;
    }
    #ifdef MMU_DEBUG
    std::cout << "Frame #" << i << " is available!" << std::endl;
    #endif
    _framesInUse[i] = true; // Set FrameTable entry to indicate frame is in use
    _pageTable[index] = i; // Set PageTable entry to refer to the frame in question
  }
  #ifdef MMU_DEBUG
  std::cout << "Page #" << index << " was referenced and has Frame #"
            << _pageTable[index] << std::endl;
  #endif
  return _pageTable[index];
}

// Prints the page table (one per process) and frame table (static for OS)
void PageTable::print()
{
  // Divider between row categories (e.g.: b/w headers and data)
  std::string rowdiv = " *------------------------------------------* \n";
  // Table headers
  std::cout << std::endl << rowdiv +
              " |   Page Table    ||    FrameInUse Table   | \n" +
              rowdiv +
              " | Page#  | Frame# || Frame# | FrameInUse?  | \n" +
              rowdiv;
  for (int i = 0; i < NUM_FRAMES; i++) {
    std::cout << " | "
              // Output page number
              << std::setw(6) << std::right << i
                << " | "
              // Output associated frame, if any
              << std::setw(6) << std::left
                << (_pageTable[i] < 0 ? "-" : std::to_string(_pageTable[i]))
                << " || "
              // Output frame number
              << std::setw(6) << std::right << i
                << " | "
              // Output whether frame is in use or free
              << std::setw(12) << std::left
                << (_framesInUse[i] ? "In use" : "Free")
                << " | "
              << std::endl;
  }
  std::cout << rowdiv << std::endl;
}
