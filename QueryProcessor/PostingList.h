#ifndef POSTINGLIST_H
#define POSTINGLIST_H

#include <vector>
#include <iostream>
#include "DataReader.h"
#include "LexiconEntry.h"

class PostingList {
private:
  int m_index;
  long long m_totalPage;
  std::vector<std::pair<int, int> > readBlock(std::ifstream &inf);
  std::pair<int, int> readLength(std::ifstream &inf);
  std::vector<std::pair<int, int> > combineDocIdFreq(std::vector<int> &docIds, std::vector<int> &freqs);

public:
  std::vector<Posting> m_postings;
  PostingList(std::ifstream &inf, LexiconEntry *entry, long long totalPage);
  int nextGEQ(int key);
  int getFreq(int key);
  int size();
};

#endif
