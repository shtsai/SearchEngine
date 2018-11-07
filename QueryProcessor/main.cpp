#include "LexiconEntry.h"
#include "DataReader.h"

using namespace std;

int main(){
  // string query;
  // cout << "Please enter your query: ";
  // getline(cin, query);
  // cout << "Your query is " << query << "\n";

  string line;
  string delimiter = ",";

  // read page table into memory
  // unordered_map<int, string> pageTable = loadPageTable("page_table.txt");

  // read term table into memory
  // unordered_map<string, int> termTable = loadTermTable("term_table.txt");

  // read lexicon into memory
  // unordered_map<int, LexiconEntry*> lexicon = loadLexicon("lexicon");

  // Test map
  // while (true) {
  //   string line;
  //   getline(cin, line);
  //   if (termTable.count(line)) {
  //     int tid = termTable[line];
  //     LexiconEntry e = *lexicon[tid];
  //     cout << e.toString() << endl;
  //   } else {
  //     cout << "Not found" << endl;
  //   }
  // }

  ifstream inf ("inverted_list", ios::binary);
  vector<pair<int, int> > res = readBlock(inf);
  for (int i = 0; i < res.size(); i++) {
    pair<int, int> q = res[i];
    cout << q.first << " + " << q.second << endl;
  }
  res = readBlock(inf);
  for (int i = 0; i < res.size(); i++) {
    pair<int, int> q = res[i];
    cout << q.first << " + " << q.second << endl;
  }
  res = readBlock(inf);
  for (int i = 0; i < res.size(); i++) {
    pair<int, int> q = res[i];
    cout << q.first << " + " << q.second << endl;
  }
}
