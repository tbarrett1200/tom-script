#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

struct SourceLocation {
  int row, col;
  SourceLocation() = default;
  SourceLocation(int r, int c) : row{r}, col{c} {};
};

struct SourceRange {
  SourceLocation start;
  SourceLocation end;
  SourceRange() = default;
  SourceRange(SourceLocation s, SourceLocation e) : start{s}, end{e} {};
};

std::ostream& operator<<(std::ostream &os, SourceLocation cat);

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

  bool commandLine;

public:
  SourceCode(const std::istream &file, std::string name) : path{name}, commandLine{false}{
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

  SourceCode() : path{"stdin"} , commandLine{true} {
    lineStarts.push_back(0);
    std::cout << "1> ";
  }

  SourceCode(std::string p) : SourceCode(std::ifstream{p}, p) {}

  std::string getPath() const {
    return path;
  }

  void reset() {
    std::cout << lineStarts.size() << "> " ;
    char c = std::cin.get();
    if (c == '\n') {
      lineStarts.push_back(text.size() + 1);
    }
    text += c;
  }

  int getLineCount() const {
    return lineStarts.size()-1;
  }

  std::string getLine(int i) const {
    if (i > getLineCount()) {
      std::stringstream ss;
      ss << "line out of bounds " << i << " is greater than " << getLineCount() << std::endl;
      throw std::string(ss.str());
    } else if (i == getLineCount()) {
      return text.substr(lineStarts[i], text.size()-2);
    } else {
      return text.substr(lineStarts[i], lineStarts[i+1]-2);
    }
  }

  char getChar(int i) {
    if (commandLine) {
      if (i > text.size() - 1) std::cout << lineStarts.size() << ". " ;

      while (i > text.size() - 1) {
        if (std::cin.eof()) return -1;
        text += std::cin.get();
        if (text.back() == '\n') lineStarts.push_back(text.size());
      }

      if (text.back() != '\n') {
        do {
          text += std::cin.get();
        } while (text.back() != '\n');
        lineStarts.push_back(text.size());
      }

      return text[i];
    } else {
      if (i>=text.size()) return -1;
      else return text[i];
    }
  }

};

class SourceManager {
public:
  static std::shared_ptr<SourceCode> currentSource;
  static std::string currentFile();
};

#endif
