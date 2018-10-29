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
  unordered_map<int, LexiconEntry*> lexicon = loadLexicon("lexicon.txt");

  // Test map
  while (true) {
    string line;
    getline(cin, line);
    int v = stoi(line);
    LexiconEntry *le = lexicon[v];
    if (le) {
      LexiconEntry e = *le;
      cout << e.toString() << endl;
    }
  }
}
