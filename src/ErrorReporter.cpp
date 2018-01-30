#include "ErrorReporter.h"

#include <iostream>

ErrorReporter::ErrorReporter(SourceCode *src) : source{src} {}

void ErrorReporter::report(int r, int c, std::string err) {
  if (!ErrorReporter::mute) {
    globalStream << source->getPath() << ":" << r << ":"<< c << ": " << err << std::endl;
    globalStream << source->getLine(r) << std::endl;
    globalStream << std::string(c, ' ') << '^' << std::endl;
  }
}

void ErrorReporter::report(Token t, std::string err) {
  report(t.row, t.col, err);
}


bool ErrorReporter::mute = false;
std::ostream &ErrorReporter::globalStream = std::cerr;
