#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H

#include <string>
#include <vector>

class FileBuffer {
public:
  std::string path;
  std::vector<std::string> lines;
  FileBuffer(std::string);
  std::string getLine(int);
};

#endif
