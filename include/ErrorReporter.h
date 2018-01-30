#ifndef ERROR_REPORT_H
#define ERROR_REPORT_H

#include <ostream>
#include "SourceCode.h"
#include "Token.h"

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
  ErrorReporter(SourceCode*);

  // reports an error to the command line with a specified row, col, and message
  // this should not be used in most cases, but is provided just in case a Token
  // object is not avaiable.
  void report(int, int, std::string);

  // reports an error to the command line taking location information from a
  // given token, and displaying the given error message.
  void report(Token, std::string);

  static std::ostream &globalStream;
  static bool mute;
};

#endif
