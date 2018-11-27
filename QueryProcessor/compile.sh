#!/bin/bash

g++ -std=c++11 -O2 -o queryProcessor main.cpp DataReader.cpp LexiconEntry.cpp QueryParser.cpp PostingList.cpp

cp queryProcessor ../run/queryProcessor
