#include "DataReader.h"
#include <iostream>
#include <bitset>

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
    while (getline(lexiconFile, line)) {
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
    }
    lexiconFile.close();
    cout << "Lexicon loaded\n";
  }
  return lexicon;
}

// Search for the given entry in the inverted list
// vector<Posting> search(LexiconEntry* entry, string filename) {
//   LexiconEntry e = *entry;
//   int termId = e.getTermId();
//   int blockId = e.getBlockId();
//   int offset = e.getOffset();
//   int length = e.getLength();
//
//   ifstream inf(filename);
//   if (!inf) {
//     cout << "Fail to open inverted list" << endl;
//   } else {
//     for (int i = 0; i < blockId; i++) {
//       // TODO read inverted list
//     }
//   }
// }

pair<int, int> readLength(ifstream &inf) {
  bool end = false;
  int sizeLength = 0;
  int value = 0;
  char c;
  while (!end) {
    inf.get(c);
    // bitset<8> x(c);
    // cout << x << " ";
    int v = (int) c & 0x7f;
    value = value * 128 + v;
    if ((c & 0x80) == 0) {
      end = true;
    }
    sizeLength++;
  }
  // cout << endl;
  return make_pair(sizeLength, value);
}

vector<pair<int, int> > readBlock(ifstream &inf) {
  pair<int, int> lengthPair = readLength(inf);
  int sizeLength = lengthPair.first;
  int blockSize = lengthPair.second;

  vector<int> docIds;
  vector<int> freqs;
  char c;
  int value = 0;
  while (blockSize > 0 && inf.get(c)) {
    // bitset<8> x(c);
    // cout << x << " ";
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
  // cout << endl;

  vector<pair<int, int> > res = combineDocIdFreq(docIds, freqs);
  return res;
}

int readCompressedBlock(char chars[], int start, int count, vector<int> &res) {
  while (count > 0) {
    bool end = false;
    int value = 0;
    char c;
    while (!end) {
      c = chars[start];
      start++;
      int v = (int) c & 0x7f;
      value = value * 128 + v;
      if ((c & 0x80) == 0) {
        end = true;
      }
    }
    res.push_back(value);
    count--;
  }
  return start;
}

vector<pair<int, int> > combineDocIdFreq(vector<int> &docIds, vector<int> &freqs) {
  vector<pair<int, int> > res;
  int prev = 0;
  for (int i = 0; i < docIds.size(); i++) {
    prev += docIds[i];
    res.push_back(make_pair(prev, freqs[i]));
  }
  return res;
}
