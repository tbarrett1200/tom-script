#include "FileBuffer.h"

#include <fstream>
#include <iostream>

FileBuffer::FileBuffer(std::string p) : path{p} {
  std::ifstream disk{p};
  while (!disk.eof()) {
    std::string line;
    std::getline(disk, line);
    lines.push_back(line);
  }
}

std::string FileBuffer::getLine(int i) {
  return lines[i];
}
