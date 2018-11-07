#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <bitset>
#include "LexiconEntry.h"

using namespace std;

// struct Posting {
//   int docId;
//   int freq;
// };

unordered_map<int, string> loadPageTable(string filename);
unordered_map<string, int> loadTermTable(string filename);
unordered_map<int, LexiconEntry*> loadLexicon(string filename);
// vector<Posting> search(LexiconEntry* entry, string filename);
vector<pair<int, int> > readBlock(ifstream &inf);
int readCompressedBlock(char chars[], int start, int count, vector<int> &res);
vector<pair<int, int> > combineDocIdFreq(vector<int> &docIds, vector<int> &freqs);
