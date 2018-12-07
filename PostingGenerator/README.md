# Posting Generator

This component processes the crawled web data (in WARC format), parses the HTML contents, extracts useful informations, and generates Postings for every word occurrence. 
Since this part is I/O intensive, it is implemented using multithreading. 

Posting Generator is also responsible for building important information such as term table, URL table, etc, which will later be used by the query processor.

## Implementation Details

### Programming Language

The Posting Generator is implemented in Java, because there are many useful Java libraries out there that can make this implementation easier, such as Jwat (for parsing WARC files) and Jsoup (for parsing HTML).

In addition, Java provides an easy way to write multithread programs, and it also comes with a lot of thread safe data structures that we can use.

### Work Flow

The Posting Generator takes a list of WARC files as input, and then it assigns jobs to ReaderManager, Readers and Workers.

There are three types of threads (roles):

- **ReaderManager**: There is one ReaderManager that manages all the Readers. It divides the workloads and assigns WARC files to the Readers.
  It maintains a queue that is shared by all the readers. This queue is used for storing parsed documents (called Page). 
  It also maintains a global URL map so that every document is assigned an unique docId. 

- **Readers**: These are the threads that actually reads the WARC files. They parse the html files into Page objects, and then store them into the shared queue.

- **Workers**: Workers are responsible for processing the Pages in the queue. Workers share a global Term map and assign each word a unique global termId. 
  They break the html texts into lowercase words, and compute the frequency of each word. Then they generate postings for each word in the format of (termId, docId, freq). 
  The postings are sorted by termId and write to the disk in chunks. 
  
  Workers also writes the parsed documents to disk so later they can be used for snippet generation.

### Configuration

The number of Readers and Workers are configurable. In general, since Readers' job is I/O intensive and likely to become the bottleneck, it is recommended to have more Workers than Readers.

You can also configure the number of Posting in each chunk. This affects the number of Posting files and the size of each one generated.

## Authors

* **Shang-Hung Tsai**
