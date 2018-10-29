#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "LexiconEntry.h"

using namespace std;

unordered_map<int, string> loadPageTable(string filename);
unordered_map<string, int> loadTermTable(string filename);
unordered_map<int, LexiconEntry*> loadLexicon(string filename);
