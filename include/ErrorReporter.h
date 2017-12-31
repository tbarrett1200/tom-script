#ifndef ERROR_REPORT_H
#define ERROR_REPORT_H

#include "FileBuffer.h"
#include "Token.h"
class ErrorReporter {
  FileBuffer &file;
public:
  ErrorReporter(FileBuffer&);
  void report(int, int, std::string);
  void report(Token, std::string);

};

#endif
