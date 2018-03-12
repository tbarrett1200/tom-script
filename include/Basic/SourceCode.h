#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

// Stores the source code of a single file and provides common access requests
// to the file.
class SourceCode {
private:
  // the path of the source code file to be read
  std::string path;
  // the raw file stored in a string.
  std::string text;
  // the indices of line starts
  std::vector<int> lineStarts;
public:

  SourceCode(const std::istream &file, std::string name) : path{name} {
    //reads the contents of a file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    text = buffer.str();

    //identifies the start location of each line of the file
    size_t last = 0;
    lineStarts.push_back(0);
    while ((last = text.find('\n', last+1)) != std::string::npos) {
      lineStarts.push_back(last+1);
    }
    lineStarts.push_back(text.length());
  }

  SourceCode(std::string p) : SourceCode(std::ifstream{p}, p) {}

  std::string getPath() {
    return path;
  }

  std::string getText() {
    return text;
  }

  int getLineCount() {
    return lineStarts.size()-1;
  }

  std::string getLine(int i) {
    if (i > getLineCount()-1) {
      throw std::out_of_range("line out of bounds");
    } else {
      const int start = lineStarts[i];
      const std::string raw = text.substr(start, lineStarts[i+1]-start);
      const std::string line = raw.back() == '\n' ? raw.substr(0, raw.length()-1) : raw;
      return line;
    }
  }

  int getLength(int) {
    return text.length();
  }

  char getChar(int i) {
    if (i>=text.size()) return -1;
    else return text[i];
  }

};

#endif
