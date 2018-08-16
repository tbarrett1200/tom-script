#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
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

struct StringRef {
  const char *start;
  int length;

  std::string str() const {
    return std::string(start, length);
  }

  StringRef(const char *s, int l): start{s}, length{l} {}
  StringRef(const char *s): start{s}, length{static_cast<int>(strlen(s))} {}
};

bool operator==(const StringRef& str1, const StringRef& str2);
bool operator<(const StringRef& str1, const StringRef& str2);
bool operator!=(const StringRef& str1, const StringRef& str2);

std::ostream& operator<<(std::ostream &stream, const StringRef& ref);

class SourceFile {
private:
  std::string path_;
  std::string contents_;
  std::vector<int> line_starts_;

  void initialize_line_starts() {
    line_starts_.push_back(0);
    for (auto it{contents_.begin()}; it != contents_.end(); it++) {
      if (*it == '\n' && std::distance(it, contents_.end()) != 1) {
        line_starts_.push_back(std::distance(contents_.begin(), it) + 1);
      }
    }
  }

public:
  SourceFile(std::istream &stream): path_{"unknown"} {
    // checks size of file, and reserves that much space to string
    stream.seekg(0, stream.end);
    contents_.reserve(stream.tellg());
    stream.seekg(0, stream.beg);
    // copies contents of file into string
    contents_.assign(
      std::istreambuf_iterator<char>{stream}
    , std::istreambuf_iterator<char>{}
    );
    initialize_line_starts();
  }

  SourceFile(std::string path): path_{path} {
    // opens a file in read mode with the given path
    std::fstream file{path, std::fstream::in};
    // checks that the file opened properly
    if (file.is_open()) {
      // checks size of file, and reserves that much space to string
      file.seekg(0, file.end);
      contents_.reserve(file.tellg());
      file.seekg(0, file.beg);
      // copies contents of file into string
      contents_.assign(
        std::istreambuf_iterator<char>{file}
      , std::istreambuf_iterator<char>{}
      );
      file.close();
      initialize_line_starts();
    } else {
      throw std::runtime_error("error: unable to open file " +  path);
    }
  }

  ~SourceFile() = default;

  int line_count() const {
    return line_starts_.size();
  }

  const std::vector<int>& line_starts() const {
    return line_starts_;
  }

  std::string::const_iterator begin() const {
    return contents_.begin();
  }
  std::string::const_iterator end() const {
    return contents_.end();
  }

  int content_length() const {
    return contents_.length();
  }

  SourceLocation location(const char* ptr) const {
    int loc = ptr - contents_.data();

    if (loc < 0 || loc >= contents_.size()) {
      throw std::out_of_range("error: pointer out of range");
    }

    for (auto it = line_starts_.begin(); it != line_starts_.end(); it++) {
      if (*it < loc) {
        int row = std::distance(line_starts_.begin(), it);
        return SourceLocation(row, loc - *it);
      }
    }

    throw std::logic_error("error: unreachable");
  }

  StringRef substr(std::pair<int, int> start, std::pair<int, int> end) {
    int range_start = line_starts_[start.first] + start.second;
    int range_end = line_starts_[end.first] + end.second;
    return {&contents_[range_start], range_end - range_start};
  }
  StringRef substr(int start, int length) const {
    return {&contents_[start], length};
  }

  StringRef line(int num) const {
    if (num == line_count() - 1) {
      int start = line_starts_[num];
      int length = contents_.length() - start;
      return substr(line_starts_[num], length);
    } else if (num < line_count() - 1 && num >= 0){
      int start = line_starts_[num];
      int length = line_starts_[num + 1] - start;
      return substr(line_starts_[num], length);
    } else {
      std::stringstream ss;
      ss << "error: line number " << num << " is out of range";
      throw std::out_of_range(ss.str());
    }
  }

  std::string path() const {
    return path_;
  }
};

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
    if (i > getLineCount() || i < 0) {
      return "";
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
  static std::shared_ptr<SourceFile> currentSource;
  static std::string currentFile();
};

#endif
