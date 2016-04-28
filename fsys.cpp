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
  // cout << this->toString() << endl;
  cout << textbox("Printing the FAT");
  string out;
  out = "*" + padding(28, '-') + "*\n";
  out += "| NAME\t\tPOS\tSIZE |\n";
  out += "*" + padding(28, '-') + "*";
  printf("\n%s", out.c_str());
  for (int i = 0; (unsigned long)i < fileTable.size(); i++) {
    FAT_Record temp = fileTable[i];
    printf("\n| %10s %7d %6d |", temp.fileName.c_str(), temp.location, temp.size);
  }
  out = "*" + padding(28, '-') + "*\n";
  printf("\n%s", out.c_str());
}

void FSYS::printDisk(int x) {
  // x will refer to either a full disk dump or only occuipied space Dump
  if (x == 0) {
    cout << textbox("PRINTING THE DISK");
    cout << setfill('0');
    cout << "*" << padding(78, '-') << "*" << endl;
    cout << "|" << padding(32) << "Disk contents" << padding(31) << "|" << endl;
    cout << "*" << padding(78, '-') << "*" << endl;
    cout << "|   Block  |"; for (int i = 0; i < 4; i++) cout << "|   Word Value  |";
    cout << endl << "*" << padding(78, '-') << "*" << endl;
    for (int i = 0; i < 64; i++) {
        cout << "|    " << setw(2) << i << "    |";
        cout << hex;
        for (int j = 0; j < 4; j++) {
          cout << "|  0x" << setw(2) << (i*4)+j
               << ": 0x" << setw(4) << disk[(i*4)+j] << " |";
        }
        cout << dec << endl;
        usleep(10000);
    }
    cout << "*" << padding(78, '-') << "*" << endl;
  }
  if (x == 1) {
    cout << textbox("PRINTING THE DISK (IN USE ONLY)");
    cout << setfill('0');
    cout << "*" << padding(78, '-') << "*" << endl;
    cout << "|" << padding(32) << "Disk contents" << padding(31) << "|" << endl;
    cout << "*" << padding(78, '-') << "*" << endl;
    cout << "|   Block  |"; for (int i = 0; i < 4; i++) cout << "|   Word Value  |";
    cout << endl << "*" << padding(78, '-') << "*" << endl;
    string output;
    bool print;
    for (int i = 0; i < 64; i++) {
        print = false;
        for (int j = 0; j < 4; j++) {
          if (disk[(i*4)+j] != 0)
            print = true;
        }
        if (print == true) {
        cout << "|    " << setw(2) << i << "    |";
        cout << hex;
        for (int j = 0; j < 4; j++) {
          cout << "|  0x" << setw(2) << (i*4)+j
               << ": 0x" << setw(4) << disk[(i*4)+j] << " |";
        }
        cout << dec << endl;
        usleep(10000);
    }
  }
    cout << "*" << padding(78, '-') << "*" << endl;
  }
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
  printDisk(1);
}

void FSYS::loadFAT() {
  string out;
  cout << textbox("Intializing the FAT with the FAT data file");
  string filename = "FAT.dat";
  string line;
  ifstream infile;
  infile.open(filename.c_str());
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
      // cout << name << " " << position << " " << size << endl;
      fileTable.push_back(newRec);
      usleep(1000000);
      out = "*" + padding(26, '-') + "*\n";
    	out += "| New File Addition to FAT |\n";
      out += "| NAME          POS    SIZE|\n";
    	out += "*" + padding(26, '-') + "*\n";
      printf("\n%s", out.c_str());
      printf("| %10s %6d %6d|", name.c_str(), position, size);
      out = "*" + padding(26, '-') + "*\n";
      printf("\n%s", out.c_str());

  }
  infile.close();
  usleep(1000000);
  print();

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
