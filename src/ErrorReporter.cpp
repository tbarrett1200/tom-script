#include "ErrorReporter.h"

#include <iostream>

ErrorReporter::ErrorReporter(FileBuffer &f) : file{f} {}

void ErrorReporter::report(int r, int c, std::string err) {
  std::cout << file.path << ":" << r << ":"<< c << ": " << err << std::endl;
  std::cout << file.getLine(r) << std::endl;
  std::cout << std::string(c, ' ') << '^' << std::endl;
}

void ErrorReporter::report(Token t, std::string err) {
  report(t.row, t.col, err);
}
