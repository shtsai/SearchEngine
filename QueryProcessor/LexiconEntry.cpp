#include "LexiconEntry.h"
#include <sstream>

LexiconEntry::LexiconEntry(int _termId, int _blockPosition, int _offset, int _length) {
  termId = _termId;
  blockPosition = _blockPosition;
  offset = _offset;
  length = _length;
}

std::string LexiconEntry::toString() {
  std::ostringstream os;
  os << "(termId=" << getTermId() << ", blockPosition=" << getBlockPosition() << ", offset=" << getOffset() << ", length=" << getLength() << ")";
  return os.str();
}
