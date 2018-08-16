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
  const SourceFile& source;

public:
  // constructs an ErrorReporter object for the given file buffer.
  ErrorReporter(std::ostream& stream, const SourceFile& src) : stream{stream}, source{src} {}

  void report(CompilerException err) {
    if (err.loc) {
      SourceLocation location{source.location(err.loc)};
      stream << source.path() << ":" << location.row << ":"<< location.col << ": " << err.category << ": " << err.message << std::endl;
      stream << source.line(location.row) << std::string(location.col, ' ') << "^" << std::endl;
    } else {
      stream << source.path() << ": "<< err.category << ": " << err.message << std::endl;
    }
  }

};

#endif
