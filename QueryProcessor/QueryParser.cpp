#include "QueryParser.h"

using namespace std;

vector<string> getANDKeywords() {
  cout << "Enter AND keywords: " << endl;
  return getInputs();
}
vector<string> getORKeywords() {
  cout << "Enter OR keywords: " << endl;
  return getInputs();
}

// Get user input and split words
vector<string> getInputs() {
  vector<string> res;
  string line;
  getline(cin, line);
  if (line.length() != 0) {
    stringstream ss(line); // Turn the string into a stream.
    string tok;
    char delimiter = ' ';
    while(getline(ss, tok, delimiter)) {
      res.push_back(tok);
    }
  }
  return res;
}
