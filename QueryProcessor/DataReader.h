#ifndef DATAREADER_H
#define DATAREADER_H

#include <unordered_map>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
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
map<int, Posting> getPostings(LexiconEntry *entry, ifstream &inf);
vector<pair<int, int> > readBlock(ifstream &inf);
vector<pair<int, int> > combineDocIdFreq(vector<int> &docIds, vector<int> &freqs);
unordered_map<int, float> getANDResult(vector<string> keywords, ifstream &invertedList, unordered_map<int, Page> &pageTable,
                         unordered_map<string, int> &termTable, unordered_map<int, LexiconEntry*> &lexicon,
                         int totalPage, int avgLength);
int nextGEQ(map<int, Posting> list, int key);
int getFreq(map<int, Posting> list, int key);
float computeBM25(int N, int ft, int fdt, int d, int davg, float k1, float b);
vector<pair<int, float> > getTop20(unordered_map<int, float> scores);
void showQueryResult(vector<pair<int, float> > results, unordered_map<int, Page> &pageTable);

#endif
