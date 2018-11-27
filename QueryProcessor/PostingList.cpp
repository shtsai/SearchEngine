#include <iostream>
#include <vector>
#include "LexiconEntry.h"
#include "PostingList.h"
#include "DataReader.h"

// Constructor
// Retrieve postings for the given entry in the inverted list
PostingList::PostingList(std::ifstream &inf, LexiconEntry *entry, long long totalPage) {
  m_index = 0;
  m_totalPage = totalPage;

  LexiconEntry e = *entry;
  int termId = e.getTermId();
  int blockPosition = e.getBlockPosition();
  int offset = e.getOffset();
  int length = e.getLength();

  // load all postings for the given entry
  inf.seekg(blockPosition);
  bool firstBlock = true;
  while (length > 0) {
    std::vector<std::pair<int, int> > block = readBlock(inf);
    int prev = 0;
    int i;
    if (firstBlock) {
      i = offset;
      firstBlock = false;
    } else {
      i = 0;
    }
    for (; i < block.size() && length > 0; i++) {
      prev += block[i].first;
      m_postings.push_back({ prev, block[i].second });
      length--;
    }
  }
}

// Read the first few bytes of an compressed block to get block length
std::pair<int, int> PostingList::readLength(std::ifstream &inf) {
  bool end = false;
  int sizeLength = 0;
  int value = 0;
  char c;
  while (!end) {
    inf.get(c);
    int v = (int) c & 0x7f;
    value = value * 128 + v;
    if ((c & 0x80) == 0) {
      end = true;
    }
    sizeLength++;
  }
  return std::make_pair(sizeLength, value);
}

// Read a block of inverted index
std::vector<std::pair<int, int> > PostingList::readBlock(std::ifstream &inf) {
  std::pair<int, int> lengthPair = readLength(inf);
  int sizeLength = lengthPair.first;
  int blockSize = lengthPair.second;

  std::vector<int> docIds;
  std::vector<int> freqs;
  char c;
  int value = 0;
  while (blockSize > 0 && inf.get(c)) {
    int v = (int) c & 0x7f;
    value = value * 128 + v;
    if ((c & 0x80) == 0) {  // reach end of a compressed value
      if (docIds.size() < 128) {
        docIds.push_back(value);
      } else {
        freqs.push_back(value);
      }
      value = 0;
    }
    blockSize--;
  }
  std::vector<std::pair<int, int> > res = combineDocIdFreq(docIds, freqs);
  return res;
}


// Zip docId and freq into a pair
std::vector<std::pair<int, int> > PostingList::combineDocIdFreq(std::vector<int> &docIds, std::vector<int> &freqs) {
  std::vector<std::pair<int, int> > res;
  for (int i = 0; i < docIds.size(); i++) {
    res.push_back(std::make_pair(docIds[i], freqs[i]));
  }
  return res;
}

int PostingList::nextGEQ(int key) {
  int left = m_index;
  int right = m_postings.size() - 1;
  while (left + 1 < right) {
    int mid = left + (right - left) / 2;
    if (m_postings[mid].docId >= key) {
      right = mid;
    } else {
      left = mid;
    }
  }
  if (m_postings[left].docId >= key) {
    m_index = left;
    return m_postings[left].docId;
  } else if (left + 1 < m_postings.size() && m_postings[left+1].docId >= key) {
    m_index = left + 1;
    return m_postings[left+1].docId;
  } else {
    m_index = m_postings.size() - 1;
    return static_cast<int>(m_totalPage);
  }
}

int PostingList::getFreq(int key) {
  // current pointer is pointing at key
  if (key == m_postings[m_index].docId) {
    return m_postings[m_index].freq;
  }

  int left = 0;
  int right = m_postings.size() - 1;
  while (left + 1 < right) {
    int mid = left + (right - left) / 2;
    if (m_postings[mid].docId == key) {
      return m_postings[mid].freq;
    } else if (m_postings[mid].docId > key) {
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }

  if (m_postings[left].docId == key) {
    return m_postings[left].freq;
  } else if (m_postings[left + 1].docId == key){
    return m_postings[left + 1].freq;
  } else {
    return 0;
  }
}

int PostingList::size() {
  return m_postings.size();
}
