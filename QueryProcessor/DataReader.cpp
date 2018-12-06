#include <iostream>
#include <bitset>
#include <climits>
#include <cmath>
#include <queue>
#include <unordered_set>
#include <chrono>
#include "DataReader.h"
#include "PostingList.h"
#include "Config.h"

using namespace std;

void showTimeElapsed(chrono::time_point<chrono::system_clock> startTime) {
  auto endTime = chrono::system_clock::now();
  chrono::duration<double> diff = endTime - startTime;
  cout << "Time elapsed: " << diff.count() * 1000 << " ms\n";
}

unordered_map<int, Page> loadPageTable(string pageTableFilename, string pageLengthFilename, int &avgLength) {
  string line;
  string delimiter = ",";
  long long pageCount = 0;
  double avg = 0.0;

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
        pageCount++;
        avg += (length - avg) / pageCount;
      } catch (invalid_argument e) {
        continue;
      }
    }
    pageLengthFile.close();
  }

  // compute average length
  avgLength = static_cast<int>(avg);
  cout << "Page table loaded, avgLength = " << avgLength << endl;
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

unordered_map<int, DocEntry> loadDocTable(string filename) {
  string line;
  string delimiter = ",";

  ifstream docTableFile (filename);
  unordered_map<int, DocEntry> docTable;
  if (docTableFile.is_open()) {
    while (getline(docTableFile, line)) {
      int start = 0;
      vector<int> values;
      try {
        for (int i = 0; i < 4; i++) {
          int comma = line.find(delimiter, start);
          values.push_back(stoi(line.substr(start, comma)));
          start = comma + 1;
        }
      } catch (invalid_argument& e) {
        cout << "ERROR: stoi, " << e.what() << ",'" << line << "'" << endl;
      }
      int docId = values[0];
      int wid = values[1];
      int startPosition = values[2];
      int length = values[3];
      docTable[docId] = { wid, startPosition, length };
    }
    docTableFile.close();
    cout << "Doc table loaded\n";
  }
  return docTable;
}

// Process AND query and compute BM25 scores
unordered_map<int, float> getANDResult(vector<string> keywords, ifstream &invertedList, unordered_map<int, Page> &pageTable,
                         unordered_map<string, int> &termTable, unordered_map<int, LexiconEntry*> &lexicon,
                         long long totalPage, int avgLength) {
  unordered_map<int, float> scores;
  if (keywords.size() == 0) {
    return scores;
  }

  // get all postings lists
  vector<PostingList> lp;
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
    PostingList postings(invertedList, le->second, totalPage);
    lp.push_back(postings);
  }

  const float k1 = 1.2;
  const float b = 0.75;

  // DAAT AND processing
  int did = 0;
  while (did <= totalPage) {
    did = lp[0].nextGEQ(did);
    // see if you find entries with same docId in the other lists
    int d = did;
    for (int i = 1; (i < lp.size()) && ((d = lp[i].nextGEQ(did)) == did); i++);
    if (d > did) {     // not in intersection
      did = d;
    } else {    // docId is in intersection, now get all frequencies
      // compute BM25 scores
      for (int i = 0; i < lp.size(); i++) {
        int freq = lp[i].getFreq(did);
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
  cout << "Find " << scores.size() << " matching results" << endl;
  return scores;
}

// Process OR query and compute BM25 scores
unordered_map<int, float> getORResult(vector<string> keywords, ifstream &invertedList, unordered_map<int, Page> &pageTable,
                                      unordered_map<string, int> &termTable, unordered_map<int, LexiconEntry*> &lexicon,
                                      long long totalPage, int avgLength) {
  unordered_map<int, float> scores;
  if (keywords.size() == 0) {
    return scores;
  }

  // get all postings lists
  vector<PostingList> lp;
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
    PostingList postings (invertedList, le->second, totalPage);
    lp.push_back(postings);
  }

  const float k1 = 1.2;
  const float b = 0.75;

  // compute BM25 scores
  for (int i = 0; i < lp.size(); i++) {
    for (auto &it : lp[i].m_postings) {
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
  cout << "Find " << scores.size() << " matching results" << endl;
  return scores;
}

float computeBM25(long long N, int ft, int fdt, int d, int davg, float k1, float b) {
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
void showQueryResult(vector<pair<int, float> > results, unordered_map<int, Page> &pageTable, unordered_map<int, DocEntry> &docTable) {
  cout << "DocID   Score    URL" << endl;
  int i = 0;
  for (const pair<int, float> &result : results) {
    int docId = result.first;
    float score = result.second;
    Page p = pageTable[docId];
    DocEntry docEntry = docTable[docId];
    string snippet = getSnippet(docEntry);
    cout << "|" << i << "|" << docId << " " << score << " " << p.url << endl;
    cout << snippet << endl;
    cout << "------------------------------------------------" << endl;
    i++;
  }
}

// Generate snippet for the given docEntry
string getSnippet(DocEntry &docEntry) {
  int wid = docEntry.workerId;
  int startPos = docEntry.startPos;
  int length = docEntry.length;

  ifstream docfile(DOC_FILE + to_string(wid) + ".txt");
  docfile.seekg(startPos);
  char *buffer = new char[length];
  docfile.read(buffer, length);
  string res(buffer);
  docfile.close();
  delete[] buffer;
  return res;
}
