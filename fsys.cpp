/*
 * fsys.cpp
 *
 * Baker, Ballard, Jager-Kujawa
 * CSC 341.001
 * Spring 2016
 */
#include "fsys.hpp"

// Constructor
FSYS::FSYS() : fileTable(FAT()), buffer(FileBuffer()) {
  load();
}

// Print method
void FSYS::print() {
  cout << this->toString() << endl;
}

// Represent the FAT table as a string
string FSYS::toString() {

}

// Save all file records out to the actual (non-virtual disk)
bool FSYS::save() {

}

// Load files from the physical disk into the virtual disk
bool FSYS::load() {
  // First we should open up the FAT.dat file and intialize the FAT_Record
  string line;
  ifstream infile("FAT.dat");
  while (getline(infile, line))
  {
      istringstream iss(line);
      short int position, size;
      string name;
      if (!(iss >> name >> position >> size)) { break; } // error
      // printf("\n\t %s, %d, %d \n", name.c_str(), position, size);

      FAT_Record newRec;
      newRec.fileName = name;
      newRec.location = position;
      newRec.size = size;

      fileTable.push_back(newRec);
  }
}

// Index the FAT using string keys (filenames)
FileBuffer FSYS::operator[](const string& key) {
  buffer.clear();
  FAT_Record record;
  bool fileExists = false;
  for (int i = 0; (unsigned long)i < fileTable.size(); i++) {
    if (my_strcasecmp(key, fileTable[i].fileName)) {
      record = fileTable[i];
      fileExists = true;
      break;
    }
  }

  if (fileExists) {
    for (int i = 0; i < record.size; i++) {
      buffer.push_back(disk[record.location + i]);
    }
  }

  return buffer;
}
