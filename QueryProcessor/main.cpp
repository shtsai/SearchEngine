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

using namespace std;

int main(){
  string line;
  string delimiter = ",";

  // load data into memory
  auto loadStartTime = chrono::system_clock::now();
  int avgLength;
  unordered_map<int, Page> pageTable = loadPageTable("page_table.txt", "page_length.txt", avgLength);
  int totalPageCount = pageTable.size();
  unordered_map<string, int> termTable = loadTermTable("term_table.txt");
  unordered_map<int, LexiconEntry*> lexicon = loadLexicon("lexicon");
  ifstream invertedList ("inverted_list", ios::binary);
  auto loadEndTime = chrono::system_clock::now();
  chrono::duration<double> loadDiff = loadEndTime - loadStartTime;
  cout << "Time to load is " << loadDiff.count() << " s\n";

  while (true) {
    // get user inputs
    vector<string> ANDkeywords = getANDKeywords();
    vector<string> ORkeywords = getORKeywords();
    cout << "Processing your query ..." << endl;

    auto queryStartTime = chrono::system_clock::now();
    cout << "here0" << endl;
    unordered_map<int, float> andResult = getANDResult(ANDkeywords, invertedList, pageTable, termTable, lexicon, totalPageCount, avgLength);
    cout << "here1" << endl;
    unordered_map<int, float> orResult = getORResult(ORkeywords, invertedList, pageTable, termTable, lexicon, totalPageCount, avgLength);
    cout << "here2" << endl;
    unordered_map<int, float> mergedResult = mergeResults(andResult, orResult);
    cout << "here3" << endl;
    vector<pair<int, float> > top20 = getTop20(mergedResult);
    cout << "here4" << endl;
    showQueryResult(top20, pageTable);
    cout << "here5" << endl;
    auto queryEndTime= chrono::system_clock::now();
    chrono::duration<double> queryDiff = queryEndTime - queryStartTime;
    cout << "Time to query is " << queryDiff.count() << " s\n";
  }

}
