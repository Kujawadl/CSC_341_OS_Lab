/*
 * utils.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341.001
 * Spring 2016
 */

#include "utils.hpp"

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
  // If index out of bounds, return -1
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

    // Random number generator setup
    std::default_random_engine rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::default_random_engine::result_type>
      rand0_63(0, 63);
    // End RNG setup

    // Find the next available frame number
    int i = rand0_63(rng);
    while (_framesInUse[i] == true) {
      #ifdef MMU_DEBUG
      std::cout << "\tFrame #" << i << " is in use..." << std::endl;
      #endif
      i = rand0_63(rng);
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
