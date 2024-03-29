#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <bitset>
#include <chrono>

#include "LexiconEntry.h"
#include "DataReader.h"
#include "QueryParser.h"
#include "Config.h"

using namespace std;

int main(){
  string line;
  string delimiter = ",";

  // load data into memory
  auto loadStartTime = chrono::system_clock::now();
  int avgLength;
  unordered_map<int, Page> pageTable = loadPageTable(PAGE_TABLE_FILE, PAGE_LENGTH_FILE, avgLength);
  long long totalPageCount = pageTable.size();
  unordered_map<string, int> termTable = loadTermTable(TERM_TABLE_FILE);
  unordered_map<int, LexiconEntry*> lexicon = loadLexicon(LEXICON_FILE);
  unordered_map<int, DocEntry> docTable = loadDocTable(DOC_TABLE_FILE);
  ifstream invertedList (INVERTED_LIST_FILE, ios::binary);
  showTimeElapsed(loadStartTime);

  while (true) {
    // get user inputs
    vector<string> ANDkeywords = getANDKeywords();
    vector<string> ORkeywords = getORKeywords();
    cout << "Processing your query ..." << endl;

    auto t1 = chrono::system_clock::now();
    unordered_map<int, float> andResult = getANDResult(ANDkeywords, invertedList, pageTable, termTable, lexicon, totalPageCount, avgLength);
    showTimeElapsed(t1);
    auto t2 = chrono::system_clock::now();
    unordered_map<int, float> orResult = getORResult(ORkeywords, invertedList, pageTable, termTable, lexicon, totalPageCount, avgLength);
    showTimeElapsed(t2);
    unordered_map<int, float> mergedResult = mergeResults(andResult, orResult);
    vector<pair<int, float> > top20 = getTop20(mergedResult);
    showQueryResult(top20, pageTable, docTable);
  }

}
