#ifndef DATAREADER_H
#define DATAREADER_H

#include <unordered_map>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include "LexiconEntry.h"

using namespace std;

struct Page {
  string url;
  int length;
  int score;
};

struct Posting {
  int docId;
  int freq;
};

struct PostingComparator {
  bool operator() (const Posting &lhs, const Posting &rhs) const {
    return lhs.docId < rhs.docId;
  }
};

unordered_map<int, Page> loadPageTable(string pageTableFilename, string pageLengthFilename, int &avgLength);
unordered_map<string, int> loadTermTable(string filename);
unordered_map<int, LexiconEntry*> loadLexicon(string filename);
set<Posting, PostingComparator> search(LexiconEntry *entry, ifstream &inf);
vector<pair<int, int> > readBlock(ifstream &inf);
vector<pair<int, int> > combineDocIdFreq(vector<int> &docIds, vector<int> &freqs);
vector<Page> getTop20(vector<string> keywords, ifstream &invertedList, unordered_map<int, Page> pageTable,
                      unordered_map<string, int> termTable, int totalPage, int avgLength);

#endif
