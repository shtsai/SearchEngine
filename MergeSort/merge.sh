#!/bin/bash

# Merge postings
sort -m -n -t ',' -k1,1 -k2,2 -o postings.txt posting*

# Merge page table
sort -m -n -t ',' -k1 -o page_table.txt page_table*

# Merge term table
sort -m -n -t ',' -k1 -o term_table.txt term_table*
