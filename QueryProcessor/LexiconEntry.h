#include <string>

#ifndef LEXICONENTRY_H
#define LEXICONENTRY_H

class LexiconEntry
{
  private:
    int termId;
    int blockPosition;
    int offset;
    int length;

  public:
    LexiconEntry() {};
    LexiconEntry(int _termId, int _blockPosition, int _offset, int _length);
    int getTermId() { return termId; }
    int getBlockPosition() { return blockPosition; }
    int getOffset() { return offset; }
    int getLength() { return length; }
    std::string toString();
};

#endif
