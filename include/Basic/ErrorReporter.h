#ifndef ERROR_REPORT_H
#define ERROR_REPORT_H

#include <ostream>
#include "Basic/SourceCode.h"
#include "Basic/CompilerException.h"
#include "Basic/Token.h"

// Reports compiler errors in a human readable format
//
// All messages are diplayed in the following format:
//  <file>:<row>:<col>: <message>
//  <line of source code>
//           ^
//
// All errors are printed to stderr. The location of the error can either be
// specified manually, or can be retrieved from a token.
class ErrorReporter {
private:
  // the stream in which to print error messages
  std::ostream& stream;

  // the file from which errors are to be reported. Supplied the lines of source
  // code for pretty printed errors.
  const SourceCode& source;

public:
  // constructs an ErrorReporter object for the given file buffer.
  ErrorReporter(std::ostream& stream, const SourceCode& src) : stream{stream}, source{src} {}

  void report(CompilerException err) {
    stream << source.getPath() << ":" << err.loc.row << ":"<< err.loc.col << ": " << err.category << ": " << err.message << std::endl;
    stream << source.getLine(err.loc.row) << std::endl;
    stream << std::string(err.loc.col, ' ') << "^" << std::endl;
  }

};

#endif
