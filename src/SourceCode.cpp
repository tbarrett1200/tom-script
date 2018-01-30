#include "SourceCode.h"

#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>

SourceCode::SourceCode(std::string p) : SourceCode(std::ifstream{p}, p) {}

SourceCode::SourceCode(const std::istream &file, std::string name) : path{name} {
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

std::string SourceCode::getPath() {
  return path;
}

std::string SourceCode::getText() {
  return text;
}

int SourceCode::getLineCount() {
  return lineStarts.size()-1;
}

std::string SourceCode::getLine(int i) {
  if (i > getLineCount()-1) {
    throw std::out_of_range("line out of bounds");
  } else {
    const int start = lineStarts[i];
    const std::string raw = text.substr(start, lineStarts[i+1]-start);
    const std::string line = raw.back() == '\n' ? raw.substr(0, raw.length()-1) : raw;
    return line;
  }
}

int SourceCode::getLength(int) {
  return text.length();
}

char SourceCode::getChar(int i) {
  if (i>=text.size()) return -1;
  else return text[i];
}
