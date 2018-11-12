#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <bitset>

#include "LexiconEntry.h"
#include "DataReader.h"

using namespace std;

int main(){
  string line;
  string delimiter = ",";

  // read page table into memory
  int avgLength;
  unordered_map<int, Page> pageTable = loadPageTable("page_table.txt", "page_length.txt", avgLength);
  int totalPageCount = pageTable.size();

  // read term table into memory
  unordered_map<string, int> termTable = loadTermTable("term_table.txt");

  // read lexicon into memory
  unordered_map<int, LexiconEntry*> lexicon = loadLexicon("lexicon");

  // open inverted list
  ifstream invertedList ("inverted_list", ios::binary);

  vector<string> keywords {"comment" , "by"};
  unordered_map<int, float> andResult = getANDResult(keywords, invertedList, pageTable, termTable, lexicon, totalPageCount, avgLength);
  vector<pair<int, float> > top20 = getTop20(andResult);
  for (auto &p : top20) {
    cout << p.first << " + " << p.second << endl;
  }
  showQueryResult(top20, pageTable);

  // while (true) {
  //   string line;
  //   getline(cin, line);
  //   int tid = stoi(line);
  //   // if (termTable.count(line)) {
  //   if (lexicon.count(tid)) {
  //     // int tid = termTable[line];
  //     LexiconEntry *e = lexicon[tid];
  //     set<Posting, PostingComparator> postings = search(e, invertedList);
  //     for (auto p : postings) {
  //       cout << p.docId << " + " << p.freq << endl;
  //     }
  //     // cout << (*e).toString() << " + " << postings.size() << endl;
  //     // vector<string> keywords {"comment"};
  //     // vector<Page> top20 = getTop20(postings, keywords, pageTable, totalPageCount, avgLength);
  //     // for (Page p : top20) {
  //     //   cout << p.url << " + " << p.score << endl;
  //     // }
  //   } else {
  //     cout << "Not found: " << tid << endl;
  //   }
  // }
}
