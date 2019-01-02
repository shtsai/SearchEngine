#!/bin/bash

# Posting Generator
# cd PostingGenerator
# mvn package
# cd ..
#
# java -cp PostingGenerator/PostingGenerator.jar PostingGenerator

# cd run
# ../MergeSort/merge.sh
# cd ..

cd IndexBuilder
./compile.sh
cd ..
cd run
echo "Building inverted index..."
./indexBuilder postings
echo "Done"
cd ..
