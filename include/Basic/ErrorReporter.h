#ifndef ERROR_REPORT_H
#define ERROR_REPORT_H

#include <ostream>
#include "Basic/SourceCode.h"
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
  // the file from which errors are to be reported. Supplied the lines of source
  // code for pretty printed errors.
  SourceCode *source;

public:
  // constructs an ErrorReporter object for the given file buffer.
  ErrorReporter(SourceCode *src) : source{src} {}

  std::string report(int r, int c, std::string err) {
    std::stringstream message;
    message << source->getPath() << ":" << r << ":"<< c << ": " << err << std::endl;
    message << source->getLine(r);
    message << std::string(c, ' ') << "^" << std::endl;
    return message.str();
  }

  std::string report(Token t, std::string err) {
    return report(t.row, t.col, err);
  }

};

#endif
