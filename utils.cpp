/*
 * utils.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341.001
 * Spring 2016
 */

#include "utils.hpp"

// Case-insensitive string comparison
bool my_strcasecmp(string str1, string str2)
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

// Convert int to string
string itos(int i) {
	stringstream ss;
	ss << i;
	return ss.str();
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
    stringstream ss;
    ss << index;
    string strIndex = ss.str();
    throw out_of_range("PageTable index " + strIndex +
                            " is invalid! Index must be 0-63!\n");
  }
  // If page table contains no frame for specified page
  if (_pageTable[index] < 0) {
    #ifdef DEBUG_VERBOSE
    cerr << "Page #" << index << " has no associated frame." << endl;
    cerr << "Searching for next available frame:" << endl;
    #endif

    // If no more available frames to allocate, throw an exception
    struct ary {
      static int* begin(int (&arr)[64]) {return arr;}
      static int* end(int (&arr)[64]) {return begin(arr) + NUM_FRAMES;}
      static int* begin(FrameTable& arr) {
        return static_cast<int*>(static_cast<void*>(arr));}
      static int* end(FrameTable& arr) {return begin(arr) + NUM_FRAMES;}
    };
    if (find(ary::begin(_pageTable), ary::end(_pageTable), false)
        == ary::end(_framesInUse)) {
      throw out_of_range("No more available frames!");
    }

    // Seed random number generator
    srand(time(NULL));

    // Find the next available frame number
    int i = rand() % 64;
    while (_framesInUse[i] == true) {
      #ifdef DEBUG_VERBOSE
      cerr << "\tFrame #" << i << " is in use..." << endl;
      #endif
      i = rand() % 64;
    }
    #ifdef DEBUG_VERBOSE
    cerr << "Frame #" << i << " is available!" << endl;
    #endif
    _framesInUse[i] = true; // Set FrameTable entry to indicate frame is in use
    _pageTable[index] = i; // Set PageTable entry to refer to the frame in question
  }
  #ifdef DEBUG_VERBOSE
  cerr << "Page #" << index << " was referenced and has Frame #"
            << _pageTable[index] << endl;
  #endif
  return _pageTable[index];
}

// Creates a string representation of the page table
string PageTable::toString()
{
  // Header
  string out = "";
	out += "*" + padding(14, '-') + "*\n";
	out += "| Page | Frame |\n";
	out += "*" + padding(14, '-') + "*\n";

	// Data
	for (int i = 0; i < NUM_FRAMES; i++) {
		stringstream ss;
		ss << dec << setfill('0');
		int frame = _pageTable[i];
		ss << "|  " << setw(2) << i << "  |  " << setw(2) << frame << "   |\n";
		out += (frame > 0 ? ss.str() : "");
	}
	out += "*" + padding(14, '-') + "*\n";

	return out;
}

void PageTable::print() {
	cout << PageTable::toString();
}

string padding(int n) { return padding(n, ' '); }
string padding(int n, char c) {
	string out = "";
	for (int i = 0; i < n; i++) {
		out += c;
	}
	return out;
}

string horizontalrule() {
	string out = "";
	for (int i = 0; i < 80; i++) {
		out += "#";
	}
	out += "\n";

	return out;
}

string textboxline(string text) {
	// Trim trailing and leading spaces
	text = text.erase(text.find_last_not_of(" \t\n\r") + 1);
	text = text.erase(0, text.find_first_not_of(" \t\n\r"));

	// Generate box
	string out = "";
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

string textbox(string text) {
	string out = "\n";
	out += horizontalrule();
	out += textboxline(text);
	out += horizontalrule();
	out += "\n";

	return out;
}

string titlebox(string text) {
	string out = "\n";
	out += horizontalrule();
	out += horizontalrule();
	out += textboxline(text);
	out += horizontalrule();
	out += horizontalrule();
	out += "\n";
	return out;
}
