#ifndef COMPILER_EXCEPTION_H
#define COMPILER_EXCEPTION_H

#include <exception>
#include <iostream>

#include "Basic/SourceCode.h"

enum class CompilerExceptionCategory {
  Info, Warning, Error
};

std::ostream& operator<<(std::ostream &os, CompilerExceptionCategory cat);


struct CompilerException: public std::exception {
public:
  std::string file;
  const char *loc;
  CompilerExceptionCategory category;
  std::string message;

  CompilerException(const char* l, std::string m)
  : file{SourceManager::currentFile()}, loc{l}, category{CompilerExceptionCategory::Error}, message{m} {};
};

std::ostream& operator<<(std::ostream &os, const CompilerException &cat);


#endif
