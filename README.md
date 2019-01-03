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

   The index builder takes the output of the Merger and builds inverted lists for all the word occurrences. 
Because the size of inverted lists is large, we use various compression techniques and compress the index by about 90%.

4. [Query Preprocessor](https://github.com/shtsai7/SearchEngine/tree/master/QueryProcessor)

   The final piece of the search engine is responsible for handling user queries and returning the top 20 results based on the ranking functions.
   The query processor also generators a snippet for each query result.
   This component is built with the goal of minimizing latency.
   
Checkout the README files in each subdirectorie for further details.

## To Run This Search Engine

There are three main steps:

1. Download crawled WARC files

   You can use the Python script download_pages.py to download WARC files. The downloaded pages will be stored under pages/ directory.
   
   The last argument specify how many WARC files you want to download. The default value 10 will be used if this argument is not specified.
   ```
   python download_pages.py 50
   ```

2. Build inverted index
   
   You can use the load.sh script to build inverted index. All the inverted index files and all intermediate files will be stored under run/ directory.
   
   This step can take up to several hours depending on your machine and the number of WARC files you have.
   
   ```
   ./load.sh
   ```
   
3. Use the search engine

   You can use the run.sh script. It will take a couple seconds for the search engine to load up. Then you can enter your query terms and the search engine will return to you the top 20 results.
   ```
   ./run.sh
   ```

## Authors

* **Shang-Hung Tsai**
