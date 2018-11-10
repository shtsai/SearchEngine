#include "DataReader.h"
#include <iostream>
#include <bitset>

using namespace std;

unordered_map<int, Page> loadPageTable(string pageTableFilename, string pageLengthFilename, int &avgLength) {
  string line;
  string delimiter = ",";
  long pageCount = 0;
  long totalLength = 0;

  ifstream pageTableFile(pageTableFilename);
  unordered_map<int, Page> pageTable;
  if (pageTableFile.is_open()) {
    while (getline(pageTableFile, line)) {
      int comma = line.find(delimiter);
      int id = stoi(line.substr(0, comma));
      string url = line.substr(comma + 1);
      pageTable[id] = { url, 0 };
    };
    pageTableFile.close();
  }

  ifstream pageLengthFile(pageLengthFilename);
  if (pageLengthFile.is_open()) {
    while (getline(pageLengthFile, line)) {
      try {
        int comma = line.find(delimiter);
        int id = stoi(line.substr(0, comma));
        int length = stoi(line.substr(comma + 1));
        pageTable[id].length = length;
        totalLength += length;
        pageCount++;
      } catch (invalid_argument e) {
        continue;
      }
    }
    pageLengthFile.close();
  }

  // compute average length
  avgLength = static_cast<int>(totalLength / pageCount);

  cout << "Page table loaded" << endl;
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
set<Posting, PostingComparator> search(LexiconEntry *entry, ifstream &inf) {
  LexiconEntry e = *entry;
  int termId = e.getTermId();
  int blockPosition = e.getBlockPosition();
  int offset = e.getOffset();
  int length = e.getLength();

  // load all postings for the given entry
  inf.seekg(blockPosition);
  set<Posting, PostingComparator> postings;
  bool firstBlock = true;
  while (length > 0) {
    vector<pair<int, int> > block = readBlock(inf);
    int i;
    if (firstBlock) {
      i = offset;
      firstBlock = false;
    } else {
      i = 0;
    }
    for (; i < block.size() && length > 0; i++) {
      postings.insert({ block[i].first, block[i].second });
      length--;
    }
  }
  return postings;
}

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

vector<pair<int, int> > combineDocIdFreq(vector<int> &docIds, vector<int> &freqs) {
  vector<pair<int, int> > res;
  int prev = 0;
  for (int i = 0; i < docIds.size(); i++) {
    prev += docIds[i];
    res.push_back(make_pair(prev, freqs[i]));
  }
  return res;
}

// vector<Page> getTop20(vector<Posting> postings, vector<string> keywords, const unordered_map<int, Page> &pageTable, int N, int davg) {
//   const float k1 = 1.2;
//   const float b = 0.75;
//   unordered_map<int, float> scores;
//   for (string t : keywords) {
//     for (Posting p : postings) {
//       int docId = p.docId;
//       int fdt = p.freq;
//       int d = pageTable[docId].length;
//       float K = k1 * ((1 - b) + b * d / davg);
//
//     }
//   }
// }
