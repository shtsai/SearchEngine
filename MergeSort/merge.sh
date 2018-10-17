#!/bin/bash

sort -n -t ',' -k1,1 -k2,2 -o postings.txt posting*
