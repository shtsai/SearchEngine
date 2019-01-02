#!/bin/bash

echo "Merging data..."

# Merge postings
sort -m -n -t ',' -k1,1 -k2,2 -o postings posting*

# Merge page table
sort -m -n -t ',' -k1 -o page_table page_table*

# Merge term table
sort -m -n -t ',' -k1 -o term_table term_table*

# Merge page length table
sort -m -n -t ',' -k1 -o page_length page_length*

# Merge doc table
sort -m -n -t ',' -k1 -o doc_table doc_table*

echo "Done!"
