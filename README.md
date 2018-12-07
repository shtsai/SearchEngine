# Search Engine

This project is a small search engine. 

The data used in this search engine are from [Common Crawl](http://commoncrawl.org/).

## Design
There are four main components of this search engine:

1. [Posting Generator](https://github.com/shtsai7/SearchEngine/tree/master/PostingGenerator)

   This component processes the crawled web data (in WARC format), parses the HTML contents, extracts useful informations, and generates Postings for every word occurrence. 
   Since this part is I/O intensive, it is implemented using multithreading. 
   
   Posting Generator is also responsible for building important information such as term table, URL table, etc, which will later be used by the query processor.
   
2. [Merger](https://github.com/shtsai7/SearchEngine/tree/master/MergeSort)
   
   Since Posting Generator is a multithread process, all the intermediate results are stored as (sorted) files on disks. 
   The Merger's job is to efficiently combine all intermediate results and sort them in an I/O efficient way.
   
3. [Index Builder](https://github.com/shtsai7/SearchEngine/tree/master/IndexBuilder)

   The index builder takes the output of the Merger and builds inverted lists for the word occurrences. Because the size of inverted lists is large, various compression techniques are used, and reads and writes are done on the byte level.

4. [Query Preprocessor](https://github.com/shtsai7/SearchEngine/tree/master/QueryProcessor)

   The final piece of the search engine is responsible for handling user queries and returning the top 20 results based on the ranking functions.
   The query processor also generators a snippet for each query result.
   This component is built with the goal of minimizing latency.
   
Checkout the README files in each subdirectorie for further details.

## Authors

* **Shang-Hung Tsai**
