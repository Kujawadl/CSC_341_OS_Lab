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

}

// Print method
void FSYS::print() {
  cout << this->toString() << endl;
}

// Represent the FAT table as a string
string FSYS::toString() {
  return "";
}

// Save all file records out to the actual (non-virtual disk)
void FSYS::save() {

}

// Load files from the physical disk into the virtual disk
void FSYS::load() {
  loadFAT();
  for (int i = 0; (unsigned long)i < fileTable.size(); i++) {
    loadFile(fileTable[i]);
  }
}

void FSYS::loadFAT() {
  string line;
  ifstream infile;
  infile.open("FAT.dat".c_str());
  while (getline(infile, line)) {
      istringstream iss(line);
      short int position, size;
      string name;
      if (!(iss >> name >> position >> size)) { break; } // error
      // printf("\n\t %s, %d, %d \n", name.c_str(), position, size);

      FAT_Record newRec;
      newRec.fileName = name;
      newRec.location = position;
      newRec.size = size;

      cout << name << " " << position << " " << size << endl;

      fileTable.push_back(newRec);
  }
  infile.close();
}

void FSYS::loadFile(FAT_Record record) {
  string line;
  char * end;
  ifstream infile;
  infile.open(record.fileName.c_str());
  int i = 0;
  while (getline(infile, line)) {
    disk[record.location + i] = strtol(line.c_str(), &end, 2);
    i++;
  }
  infile.close();
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
