# Merger

Since Posting Generator is a multithread process, all the intermediate results are stored as (sorted) files on disks. 
   The Merger's job is to efficiently combine all intermediate results and sort them in an I/O efficient way.

## Implementation Detail

### Programming Language

The Merger is written in shell script.

### Description

The Merger is fairly simple. Because the each file individually is already sorted by the Posting Generator, the Merger's job is simply to merge these sorted files.
Note that some files are sorted based on multiple keys, and we need to merge them accordingly.

We take advantage of the Unix sort utility to merge sorted file.

## Authors

* **Shang-Hung Tsai**
