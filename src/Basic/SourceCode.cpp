#include "Basic/SourceCode.h"

std::shared_ptr<SourceFile> SourceManager::currentSource = nullptr;

bool operator==(const StringRef& str1, const StringRef& str2) {
  return str1.length == str2.length
      && (str1.start == str2.start || strncmp(str2.start, str1.start, str1.length) == 0);
}

bool operator!=(const StringRef& str1, const StringRef& str2) {
  return str1.length != str2.length
      || (str1.start != str2.start && strncmp(str2.start, str1.start, str1.length) != 0);
}

bool operator<(const StringRef& str1, const StringRef& str2) {
  return strncmp(str2.start, str1.start, str1.length) < 0;
}

std::ostream& operator<<(std::ostream &stream, const StringRef& ref) {
  stream.write(ref.start, ref.length);
  return stream;
}

std::string SourceManager::currentFile() {
  if (SourceManager::currentSource != nullptr) {
    return currentSource->path();
  } else {
    throw new std::logic_error("source does not exist");
  }
}

std::ostream& operator<<(std::ostream &os, SourceLocation loc) {
  return os << loc.row << ":" << loc.col;
}
