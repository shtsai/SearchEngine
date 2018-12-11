# Query Processor

The final piece of the search engine is responsible for handling user queries and returning the top 20 results based on the ranking functions.
The query processor also generators a snippet for each query result.

This component is built with the goal of minimizing latency and return the most relevant results.

## Implementation Detail

### Programming Language

The Query Processor is also implemented in C++. The reasion is similar to the reason in Index Builder. 
We need to work on the byte level (because our inverted index is compressed and need to be processed byte by byte).
We also need performance to minimize the latency in getting search results.

### Description

#### Query types

The Query Processor currently supports two types of queries: AND and OR. AND queries require the documents to contain all keywords, while documents only need to contain any of the keywords for OR queries.

#### Retrieving PostingList

The first important step the Query Processor performs is to retrieve the PostingLists for all the keywords from the inverted index. Because the inverted index is compressed, we need some help from the Lexicon in order to read it. For each term, we have a LexiconEntry, which stores the termId, blockPosition, offset, and length. With this lexicon, we can find the starting block position, read the entire block, decompress it, and retrieve postings. We start reading from the offset position, because a block can contain postings for multiple terms. If the number of postings we get is fewer than the length, we read next block, until we find all postings for the term. After we get all the PostingLists for all the keywords, we can start processing the queries.

#### AND query

For AND queries, we use DAAT (Document At A Time) technique. Instead of scanning through all postings in the PostingList, we can find their intersections efficiently by using binary search and storing head indices of PostingLists. For each PostingList, there are two APIs, nextGEQ(did) and getFreq(). nextGEQ(did) returns the posting that has a docId greater than or equal to the target `did`. This API will move the head pointer of the list forward, and thus narrow down the search range each time. getFreq() will return the frequency value in the posting that is currently pointed by the head pointer. 

Once we find a document that is the intersection of all PostingLists, we compute its BM25 score and accumulate it in a HashMap. Finally, we can sort the results by BM25 scores and take the top 20.

#### OR query

We perform similiar operations for OR queries, except we don't use the DAAT technique to find intersections. For OR queries, all documents are relevant, so we compute BM25 score for each document and get the top 20.

#### Snippet Generation

For the top 20 query results, we generate document snippets. This is done by retrieving the html content from the html collections. Html collections are stored in a similar fashion to the inverted index. We store the starting position and the length of each document, so that we can retrieve them efficiently. This collections is built during the Posting generation process.

## Authors

* **Shang-Hung Tsai**
