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


## Authors

* **Shang-Hung Tsai**
