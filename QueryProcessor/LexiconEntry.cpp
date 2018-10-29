#include "LexiconEntry.h"
#include <sstream>

LexiconEntry::LexiconEntry(int _termId, int _blockId, int _offset, int _length) {
  termId = _termId;
  blockId = _blockId;
  offset = _offset;
  length = _length;
}

std::string LexiconEntry::toString() {
  std::ostringstream os;
  os << "(termId=" << getTermId() << ", blockId=" << getBlockId() << ", offset=" << getOffset() << ", length=" << getLength() << ")";
  return os.str();
}
