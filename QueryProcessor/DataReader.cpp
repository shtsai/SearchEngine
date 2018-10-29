#include "DataReader.h"

using namespace std;

unordered_map<int, string> loadPageTable(string filename) {
  string line;
  string delimiter = ",";

  ifstream pageTableFile(filename);
  unordered_map<int, string> pageTable;
  if (pageTableFile.is_open())
  {
    while (getline(pageTableFile, line))
    {
      int comma = line.find(delimiter);
      int id = stoi(line.substr(0, comma));
      string url = line.substr(comma + 1);
      pageTable[id] = url;
    };
    pageTableFile.close();
    cout << "Page table loaded\n";
  }
  return pageTable;
}

unordered_map<string, int> loadTermTable(string filename) {
  string line;
  string delimiter = ",";

  ifstream termTableFile (filename);
  unordered_map<string, int> termTable;
  if (termTableFile.is_open())
  {
    while (getline(termTableFile, line))
    {
      int comma = line.find(delimiter);
      int id = stoi(line.substr(0, comma));
      string term = line.substr(comma + 1);
      termTable[term] = id;
    }
    termTableFile.close();
    cout << "Term table loaded\n";
  }
  return termTable;
}

unordered_map<int, LexiconEntry*> loadLexicon(string filename) {
  string line;
  string delimiter = ",";

  ifstream lexiconFile (filename);
  unordered_map<int, LexiconEntry*> lexicon;
  if (lexiconFile.is_open()) {
    int n = 100;
    while (n > 0 && getline(lexiconFile, line)) {
      int start = 0;
      vector<int> values;
      for (int i = 0; i <= 3; i++) {
        int comma = line.find(delimiter, start);
        values.push_back(stoi(line.substr(start, comma)));
        start = comma + 1;
      }
      values.push_back(stoi(line.substr(start)));
      LexiconEntry *le = new LexiconEntry(values[0], values[1], values[2], values[3]);
      lexicon[values[0]] = le;
      n--;
    }
    lexiconFile.close();
    cout << "Lexicon loaded\n";
  }
  return lexicon;
}
