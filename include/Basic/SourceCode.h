#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H

#include <string>
#include <vector>

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
  // Constructs a SourceCode from the given path. Reads and processes file.
  SourceCode(std::string path);
  //Consturctors a SourceCode object from a stream and a name.
  SourceCode(const std::istream &file, std::string name);
  // Returns the file's path
  std::string getPath();
  // Returns a string with the entire file
  std::string getText();
  // Returns the specified line from the source code file.
  std::string getLine(int);
  // Returns the number of lines in a file
  int getLineCount();
  // Returns the length of the file
  int getLength(int);
  // Returns a char at the specified index within the file
  char getChar(int);

};

#endif
