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
    std::stringstream ss;
    ss << index;
    std::string strIndex = ss.str();
    throw std::out_of_range("PageTable index " + strIndex +
                            " is invalid! Index must be 0-63!\n");
  }
  // If page table contains no frame for specified page
  if (_pageTable[index] < 0) {
    #ifdef DEBUG_VERBOSE
    std::cout << "Page #" << index << " has no associated frame." << std::endl;
    std::cout << "Searching for next available frame:" << std::endl;
    #endif

    // If no more available frames to allocate, throw an exception
    struct ary {
      static int* begin(int (&arr)[64]) {return arr;}
      static int* end(int (&arr)[64]) {return begin(arr) + NUM_FRAMES;}
      static int* begin(FrameTable& arr) {
        return static_cast<int*>(static_cast<void*>(arr));}
      static int* end(FrameTable& arr) {return begin(arr) + NUM_FRAMES;}
    };
    if (std::find(ary::begin(_pageTable), ary::end(_pageTable), false)
        == ary::end(_framesInUse)) {
      throw std::out_of_range("No more available frames!");
    }

    // Seed random number generator
    srand(time(NULL));

    // Find the next available frame number
    int i = rand() % 64;
    while (_framesInUse[i] == true) {
      #ifdef DEBUG_VERBOSE
      std::cout << "\tFrame #" << i << " is in use..." << std::endl;
      #endif
      i = rand() % 64;
    }
    #ifdef DEBUG_VERBOSE
    std::cout << "Frame #" << i << " is available!" << std::endl;
    #endif
    _framesInUse[i] = true; // Set FrameTable entry to indicate frame is in use
    _pageTable[index] = i; // Set PageTable entry to refer to the frame in question
  }
  #ifdef DEBUG_VERBOSE
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
    std::stringstream ss;
    ss << _pageTable[i];
    std::string strFrame = ss.str();
    std::cout << " | "
              // Output page number
              << std::setw(6) << std::right << i
                << " | "
              // Output associated frame, if any
              << std::setw(6) << std::left
                << (_pageTable[i] < 0 ? "-" : strFrame)
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

std::string horizontalrule() {
	std::string out = "";
	for (int i = 0; i < 80; i++) {
		out += "#";
	}
	out += "\n";

	return out;
}

std::string textboxline(std::string text) {
	// Trim trailing and leading spaces
	text = text.erase(text.find_last_not_of(" \t\n\r") + 1);
	text = text.erase(0, text.find_first_not_of(" \t\n\r"));

	// Generate box
	std::string out = "";
	if (text.length() < 76) {
		int numSpacers = 76 - text.length();
		int leftSpacers = (numSpacers / 2);
		int rightSpacers = leftSpacers  + (numSpacers % 2);

		out += "# ";
		for (int i = 0; i < leftSpacers; i++) {
			out += " ";
		}
		out += text;
		for (int i = 0; i < rightSpacers; i++) {
			out += " ";
		}
		out += " #\n";
	} else {
		int i = (text.length() / 2) - 1;
		while (text.at(i) != '-' && text.at(i) != ' ') {
			i++;
		}
		i++;
		out += textboxline(text.substr(0, i));
		out += textboxline(text.substr(i, text.length() + 1 - i));
	}

	return out;
}

std::string textbox(std::string text) {
	std::string out = "\n";
	out += horizontalrule();
	out += textboxline(text);
	out += horizontalrule();
	out += "\n";

	return out;
}
