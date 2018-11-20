#include "DataReader.h"
#include <iostream>
#include <bitset>
#include <climits>
#include <cmath>
#include <queue>
#include <unordered_set>
#include <chrono>

using namespace std;

void showTimeElapsed(chrono::time_point<chrono::system_clock> startTime) {
  auto endTime = chrono::system_clock::now();
  chrono::duration<double> diff = endTime - startTime;
  cout << "Time elapsed: " << diff.count() * 1000 << " ms\n";
}

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

// Retrieve postings for the given entry in the inverted list
vector<Posting> getPostings(LexiconEntry *entry, ifstream &inf) {
  LexiconEntry e = *entry;
  int termId = e.getTermId();
  int blockPosition = e.getBlockPosition();
  int offset = e.getOffset();
  int length = e.getLength();

  // load all postings for the given entry
  inf.seekg(blockPosition);
  vector<Posting> postings;
  bool firstBlock = true;
  while (length > 0) {
    vector<pair<int, int> > block = readBlock(inf);
    int prev = 0;
    int i;
    if (firstBlock) {
      i = offset;
      firstBlock = false;
    } else {
      i = 0;
    }
    for (; i < block.size() && length > 0; i++) {
      prev += block[i].first;
      postings.push_back({ prev, block[i].second });
      length--;
    }
  }
  return postings;
}

// Read the first few bytes of an compressed block to get block length
pair<int, int> readLength(ifstream &inf) {
  bool end = false;
  int sizeLength = 0;
  int value = 0;
  char c;
  while (!end) {
    inf.get(c);
    int v = (int) c & 0x7f;
    value = value * 128 + v;
    if ((c & 0x80) == 0) {
      end = true;
    }
    sizeLength++;
  }
  return make_pair(sizeLength, value);
}

// Read a block of inverted index
vector<pair<int, int> > readBlock(ifstream &inf) {
  pair<int, int> lengthPair = readLength(inf);
  int sizeLength = lengthPair.first;
  int blockSize = lengthPair.second;

  vector<int> docIds;
  vector<int> freqs;
  char c;
  int value = 0;
  while (blockSize > 0 && inf.get(c)) {
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
  vector<pair<int, int> > res = combineDocIdFreq(docIds, freqs);
  return res;
}

// Zip docId and freq into a pair
vector<pair<int, int> > combineDocIdFreq(vector<int> &docIds, vector<int> &freqs) {
  vector<pair<int, int> > res;
  for (int i = 0; i < docIds.size(); i++) {
    res.push_back(make_pair(docIds[i], freqs[i]));
  }
  return res;
}

int nextGEQ(vector<Posting> list, int key) {
  int left = 0;
  int right = list.size() - 1;
  while (left + 1 < right) {
    int mid = left + (right - left) / 2;
    if (list[mid].docId >= key) {
      right = mid;
    } else {
      left = mid;
    }
  }
  if (list[left].docId >= key) {
    return list[left].docId;
  } else {
    return list[left+1].docId;
  }
}

int getFreq(vector<Posting> list, int key) {
  int left = 0;
  int right = list.size() - 1;
  while (left + 1 < right) {
    int mid = left + (right - left) / 2;
    if (list[mid].docId == key) {
      return list[mid].freq;
    } else if (list[mid].docId > key) {
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }

  if (list[left].docId == key) {
    return list[left].freq;
  } else if (list[left + 1].docId == key){
    return list[left + 1].freq;
  } else {
    return 0;
  }
}

// Process AND query and compute BM25 scores
unordered_map<int, float> getANDResult(vector<string> keywords, ifstream &invertedList, unordered_map<int, Page> &pageTable,
                         unordered_map<string, int> &termTable, unordered_map<int, LexiconEntry*> &lexicon,
                         int totalPage, int avgLength) {
  unordered_map<int, float> scores;
  if (keywords.size() == 0) {
    return scores;
  }

  // get all postings lists
  vector<vector<Posting> > lp;
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
    vector<Posting> postings = getPostings(le->second, invertedList);
    lp.push_back(postings);
  }

  const float k1 = 1.2;
  const float b = 0.75;

  // DAAT AND processing
  int did = 0;
  while (did <= totalPage) {
    did = nextGEQ(lp[0], did);
    // see if you find entries with same docId in the other lists
    int d = did;
    for (int i = 1; (i < lp.size()) && ((d = nextGEQ(lp[i], did)) == did); i++);
    if (d > did) {     // not in intersection
      did = d;
    } else {    // docId is in intersection, now get all frequencies
      // compute BM25 scores
      for (int i = 0; i < lp.size(); i++) {
        int freq = getFreq(lp[i], did);
        int docLength = pageTable[did].length;
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

// Process OR query and compute BM25 scores
unordered_map<int, float> getORResult(vector<string> keywords, ifstream &invertedList, unordered_map<int, Page> &pageTable,
                                      unordered_map<string, int> &termTable, unordered_map<int, LexiconEntry*> &lexicon,
                                      int totalPage, int avgLength) {
  unordered_map<int, float> scores;
  if (keywords.size() == 0) {
    return scores;
  }

  // get all postings lists
  vector<vector<Posting>> lp;
  for (string keyword : keywords) {
    auto termId = termTable.find(keyword);
    if (termId == termTable.end()) {   // cannot find keyword, continue searching next word
      cout << "WARN: cannot find termId for '" << keyword << "'" << endl;
      continue;
    }
    auto le = lexicon.find(termId->second);
    if (le == lexicon.end()) {    // cannot find lexicon entry for the termId, continue search next word
      cout << "WARN: cannot find lexicon entry for termId '" << termId->second << "'" << endl;
      continue;
    }
    vector<Posting> postings = getPostings(le->second, invertedList);
    lp.push_back(postings);
  }

  const float k1 = 1.2;
  const float b = 0.75;

  // compute BM25 scores
  for (int i = 0; i < lp.size(); i++) {
    for (auto &it : lp[i]) {
      int docId = it.docId;
      int freq = it.freq;
      int docLength = pageTable[docId].length;
      float bm25 = computeBM25(totalPage, lp[i].size(), freq, docLength, avgLength, k1, b);
      auto score = scores.find(docId);
      if (score == scores.end()) {
        scores.insert({ docId, bm25 });
      } else {
        score->second = score->second + bm25;
      }
    }
  }
  return scores;
}

float computeBM25(int N, int ft, int fdt, int d, int davg, float k1, float b) {
  float K = k1 * ((1 - b) + b * d / davg);
  return log(((N - ft + 0.5) / (ft + 0.5)) * ((k1 + 1) * fdt / (K + fdt)));
}

// Sort the scores and return top 20 results
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

unordered_map<int, float> mergeResults(unordered_map<int, float> ANDResult, unordered_map<int, float> ORResult) {
  unordered_map<int, float> res;
  for (auto &it : ANDResult) {
    auto e = res.find(it.first);
    if (e == res.end()) {
      res.insert( {it.first, it.second} );
    } else {
      e->second = e->second + it.second;
    }
  }
  for (auto &it : ORResult) {
    auto e = res.find(it.first);
    if (e == res.end()) {
      res.insert( {it.first, it.second} );
    } else {
      e->second = e->second + it.second;
    }
  }
  return res;
}

// Display given query results
void showQueryResult(vector<pair<int, float> > results, unordered_map<int, Page> &pageTable) {
  cout << "DocID   Score    URL" << endl;
  for (const pair<int, float> &result : results) {
    int docId = result.first;
    float score = result.second;
    Page p = pageTable[docId];
    cout << docId << " " << score << " " << p.url << endl;
  }
}
