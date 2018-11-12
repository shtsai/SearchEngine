#include "DataReader.h"
#include <iostream>
#include <bitset>
#include <climits>
#include <cmath>
#include <queue>

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
map<int, Posting> getPostings(LexiconEntry *entry, ifstream &inf) {
  LexiconEntry e = *entry;
  int termId = e.getTermId();
  int blockPosition = e.getBlockPosition();
  int offset = e.getOffset();
  int length = e.getLength();

  // load all postings for the given entry
  inf.seekg(blockPosition);
  map<int, Posting> postings;
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
      postings.insert({ block[i].first, { block[i].first, block[i].second }});
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

int nextGEQ(map<int, Posting> list, int key) {
  auto it = list.lower_bound(key);
  if (it == list.end()) {
    return INT_MAX;
  } else {
    return it->first;
  }
}

int getFreq(map<int, Posting> list, int key) {
  auto it = list.find(key);
  if (it == list.end()) {
    return 0;
  } else {
    return (it->second).freq;
  }
}

unordered_map<int, float> getANDResult(vector<string> keywords, ifstream &invertedList, unordered_map<int, Page> &pageTable,
                         unordered_map<string, int> &termTable, unordered_map<int, LexiconEntry*> &lexicon,
                         int totalPage, int avgLength) {
  unordered_map<int, float> scores;

  // get all postings lists
  vector<map<int, Posting>> lp;
  for (string keyword : keywords) {
    auto termId = termTable.find(keyword);
    if (termId == termTable.end()) {   // cannot find keyword, return empty vector
      cout << "WARN: cannot find termId for '" << keyword << "'" << endl;
      return scores;
    }
    auto le = lexicon.find(termId->second);
    if (le == lexicon.end()) {    // cannot find lexicon entry for the termId
      cout << "WARN: cannot find lexicon entry for termId '" << termId->second << "'" << endl;
      return scores;
    }
    map<int, Posting> postings = getPostings(le->second, invertedList);
    lp.push_back(postings);
  }

  const float k1 = 1.2;
  const float b = 0.75;

  // DAAT AND processing
  int did = 0;
  while (did <= totalPage) {
    did = nextGEQ(lp[0], did);
    // see if you find entries with same docId in the other lists
    int d;
    for (int i = 1; (i < lp.size()) && ((d = nextGEQ(lp[i], did)) == did); i++);
    if (d > did) {     // not in intersection
      did = d;
    } else {    // docId is in intersection, now get all frequencies
      // compute BM25 scores
      for (int i = 0; i < lp.size(); i++) {
        int freq = getFreq(lp[i], did);
        int docLength = pageTable[did].length;
        // float bm25 = computeBM25(totalPage, (*lp[i]).size(), freq, docLength, avgLength, k1, b);
        float bm25 = computeBM25(totalPage, lp[i].size(), freq, docLength, avgLength, k1, b);
        auto it = scores.find(did);
        if (it == scores.end()) {
          scores.insert({ did, bm25 });
        } else {
          it->second = it->second + bm25;
        }
      }
      did++;
    }
  }
  return scores;
}

float computeBM25(int N, int ft, int fdt, int d, int davg, float k1, float b) {
  float K = k1 * ((1 - b) + b * d / davg);
  return log(((N - ft + 0.5) / (ft + 0.5)) * ((k1 + 1) * fdt / (K + fdt)));
}

vector<pair<int, float> > getTop20(unordered_map<int, float> scores) {
  auto comp = []( pair<float, int> a, pair<float, int> b ) { return a.first > b.first ; };
  priority_queue<pair<float, int>, vector<pair<float, int> >, decltype(comp)> pq (comp);
  for (auto score : scores) {
    if (pq.size() < 20) {
      pq.push(make_pair(score.second, score.first));
    } else if (score.second > pq.top().first) {
      pq.pop();
      pq.push(make_pair(score.second, score.first));
    }
  }
  vector<pair<int, float> > res;
  while (pq.size() != 0) {
    pair<float, int> p = pq.top();
    res.insert(res.begin(), make_pair(p.second, p.first));
    pq.pop();
  }
  return res;

}

void showQueryResult(vector<pair<int, float> > results, unordered_map<int, Page> &pageTable) {
  cout << "DocID   Score    URL" << endl;
  for (const pair<int, float> &result : results) {
    int docId = result.first;
    float score = result.second;
    Page p = pageTable[docId];
    cout << docId << " " << score << " " << p.url << endl;
  }
}
