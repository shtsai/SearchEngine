#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

vector<int> split(string &line, const string &delimiter, int len) {
  int start = 0;
  vector<int> values;
  try {
    for (int i = 0; i < len - 1; i++) {
      int comma = line.find(delimiter, start);
      values.push_back(stoi(line.substr(start, comma)));
      start = comma + 1;
    }
    values.push_back(stoi(line.substr(start)));
  } catch (invalid_argument& e) {
    cout << "ERROR: stoi, " << e.what() << ", '" << line << "'" << endl;
    values.clear();
  }
  return values;
}

vector<char> vbyteCompress(int num) {
  if (num == 0) {
    vector<char> values {0};
    return values;
  }

  vector<char> values;
  while (num > 0) {
    char lower = static_cast<char>(num & 0x7f);
    num = num >> 7;
    values.push_back(lower);
  }
  vector<char> res;
  int len = values.size();
  for (int i = 0; i < len; i++) {
    int v = values[len - 1 - i];
    if (i + 1 < len) {  // signal more coming
      v |= 0x80;
    }
    res.push_back(v);
  }
  return res;
}

// int compressWrite(ofstream &invertedList, int curBlockPosition, vector<int> &docIds, vector<int> &freqs) {
int compressWrite(ofstream &invertedList, vector<int> &docIds, vector<int> &freqs) {
  // invertedList.seekp(curBlockPosition);
  vector<char> docIdsCompressed;
  vector<char> freqsCompressed;
  for (int di : docIds) {
    vector<char> vcs = vbyteCompress(di);
    docIdsCompressed.insert(docIdsCompressed.end(), vcs.begin(), vcs.end());
  }
  for (int f : freqs) {
    vector<char> vcs = vbyteCompress(f);
    freqsCompressed.insert(freqsCompressed.end(), vcs.begin(), vcs.end());
  }
  int size = docIdsCompressed.size() + freqsCompressed.size();
  vector<char> compressedSize = vbyteCompress(size);
  for (char s : compressedSize) {
    invertedList.put(s);
    // bitset<8> x(s);
    // cout << x << " ";
  }
  for (char di : docIdsCompressed) {
    invertedList.put(di);
    // bitset<8> x(di);
    // cout << x << " ";
  }
  for (char f : freqsCompressed) {
    invertedList.put(f);
    // bitset<8> x(f);
    // cout << x << " ";
  }
  // cout << endl;
  return compressedSize.size() + size;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    cout << "ERROR: Missing input posting file" << endl;
    return 1;
  }

  // open write file
  ofstream invertedList("inverted_list", ios::binary);
  ofstream lexicon("lexicon", ios::binary);
  if (!invertedList || !lexicon) {
    cout << "ERROR: Cannot open write file" << endl;
    return 1;
  }

  int curTermId = -1;
  int curOffset = 0;
  int curLength = 0;
  int curStartBlockPosition = 0;
  int curBlockPosition = 0;
  int prevDocId = 0;
  vector<int> docIds;
  vector<int> freqs;

  ifstream postingsFile(argv[1]);
  if (!postingsFile) {
    cout << "ERROR: cannot open postings file" << endl;
    return 1;
  }
  string st;
  while (postingsFile) {
    getline(postingsFile, st);
    vector<int> posting = split(st, ",", 3);
    if (posting.empty()) {
      continue;
    }
    int termId = posting[0];
    int docId = posting[1];
    int freq = posting[2];

    if (termId != curTermId) {
      if (curTermId != -1) {
        lexicon << curTermId << "," << curStartBlockPosition << "," << curOffset << "," << curLength << endl;
      }
      curStartBlockPosition = curBlockPosition;
      curOffset = docIds.size();
      curTermId = termId;
      curLength = 0;
      prevDocId = 0;
    }

    docIds.push_back(docId - prevDocId);
    prevDocId = docId;
    freqs.push_back(freq);
    curLength++;

    if (docIds.size() == 128) {
      // curBlockPosition += compressWrite(invertedList, curBlockPosition, docIds, freqs);
      curBlockPosition += compressWrite(invertedList, docIds, freqs);
      docIds.clear();
      freqs.clear();
      prevDocId = 0;
    }
  }

  // write entry for last term
  if (curTermId != -1) {
    lexicon << curTermId << "," << curStartBlockPosition << "," << curOffset << "," << curLength << endl;
  }
  lexicon.close();
  invertedList.close();
}
