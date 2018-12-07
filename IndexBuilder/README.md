# Index Builder

The index builder takes the output of the Merger and builds inverted lists for all the word occurrences. 
Because the size of inverted lists is large, we use various compression techniques and compress the index by about 90%.

## Inplementation Detail

### Programming Language

The index builder is implemented in C++. The main reason for using C++ here is performance. To process huge amount (GBs) of inverted list, we need something performant.

In addition, it is easier to work on byte level in C++, which is good for performing compression.

### Description

The input to the Index Builder is the sorted postings produced by the Merger. Here, we are compressing them so that we can have a reasonablely sized index.

Origin format:
```
# Each line is a Posting
# (TermId, DocId, Freq)
1, 3, 2
1, 5, 1
1, 9, 2
1, 11, 1
2, 1, 2
2, 3, 1
```

We combine all the postings of the same TermId in a list.

```
# Each line is a Posting list
(3, 2) (5, 1) (9, 2) (11, 1)     # For TermId 1
(1, 2) (3, 1)                    # For TermId 2
```

Since a posting list can be very long, we need to break them into smaller chunks. Each chunk will contain 128 postings.

Within in each chunk, we group all the DocIds together and group all the Freqs together.

```
# (DocIds, Freqs)
(3, 5, 9, 11) (2, 1, 2, 1)       # For TermId 1
(1, 3) (2, 1)                    # For TermId 2
```

Because the DocIds are stored in ascending order, we can further compress the DocIds by taking the difference between adjacent docIds. 
This way makes the values we need to store much smaller. We can restore them by summing them back together.

Freqs cannot be compressed this way because they are no sorted. However, most Freq values should be relatively smaller compared to DocId.

```
(3, 2, 4, 2) (2, 1, 2, 1)        # For TermId 1
(1, 2) (2, 1)                    # For TermId 2
```

Finally, we can compress the values using [variable byte codes](https://nlp.stanford.edu/IR-book/html/htmledition/variable-byte-codes-1.html) compression technique. 
This is because each int variable takes 4 bytes of memory, but most values we have are very small and can fit in 1-2 bytes. 

For each block, we prepend the length of the block in bytes. Therefore, by reading the first compressed number, we can know how long this block is, fetch the entire block and decompress it.

To effciently look up a given term, we do the following.

For each TermId, we store its start position (in bytes) and its offset in the block (because a block can contain postings for multiple terms). We can then use seekg() function in C++ to move our read head to that position.

In addition, we store the total number of postings for this term. We can then search block by block until we find all postings.

## Authors

* **Shang-Hung Tsai**
