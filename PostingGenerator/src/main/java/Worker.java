import java.io.*;
import java.util.*;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;

import org.jwat.warc.WarcRecord;

public class Worker extends Thread {
    public static ConcurrentHashMap<String, Integer> termMap = new ConcurrentHashMap<>();
    private static int termId = 0;
    private PrintWriter termIdWriter;

    private ReaderManager rm;
    private BlockingQueue<Page> queue;
    private int wid;
    private int postingSize;
    private int fileIndex;
    private int curCount;
    private int curPosition;
    private PrintWriter postingWriter;
    private PrintWriter lengthWriter;
    private PrintWriter docWriter;
    private PrintWriter docTableWriter;

    public Worker(ReaderManager rm, int wid, int postingSize) {
        super();
        this.rm = rm;
        this.queue = rm.getQueue();
        this.wid = wid;
        this.postingSize = postingSize;
        this.fileIndex = 0;
        this.curCount = 0;
        this.curPosition = 0;
        try {
            termIdWriter = new PrintWriter("term_table" + this.wid + ".txt", "utf-8");
            postingWriter = new PrintWriter("posting-" + this.wid + "-" + this.fileIndex + ".txt", "utf-8");
            lengthWriter = new PrintWriter("page_length-" + this.wid + ".txt", "utf-8");
            docWriter = new PrintWriter("document" + this.wid + ".txt", "utf-8");
            docTableWriter = new PrintWriter("doc_table" + this.wid + ".txt", "utf-8");
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void run() {
        List<Posting> res = new ArrayList<>();
        while (!rm.isFinished()) {
            Page p = queue.poll();
            if (p == null) {
                try {
                    Thread.sleep(100);
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
                continue;
            }
            int docId = p.getDocId();
//            String url = p.getUrl();
//            WarcRecord record = p.getRecord();
            String html = p.getHtml();
            if (html == null) {
                continue;
            }

            System.out.println("processing doc " + docId);
            String[] words = html.split("[\\p{Punct}\\s]+");
            int wordCount = 0;

            HashMap<Integer, Integer> freq = new HashMap<>();
            StringBuilder sb = new StringBuilder();
            for (String w : words) {
                if (isAscii(w)) {
                    wordCount++;
                    w = w.toLowerCase();
                    Integer tid = null;
                    while (tid == null) {
                        tid = getTermId(w);
                    }
                    freq.put(tid, freq.getOrDefault(tid, 0) + 1);
                    sb.append(w + " ");
                }
            }

            // write html content to document file for snippet generation
            docWriter.print(sb.toString());
            docTableWriter.println(docId + "," + wid + "," + curPosition + "," + sb.length());
            curPosition += sb.length();

            // create postings
            for (Integer ti : freq.keySet()) {
                res.add(new Posting(ti, docId, freq.get(ti)));
            }

            // write postings in chunks
            if (res.size() >= postingSize) {
                Collections.sort(res);
                for (Posting po : res) {
                    postingWriter.println(po.toString());
                }
                res = new ArrayList<>();
                fileIndex++;
                postingWriter.close();
                try {
                    postingWriter = new PrintWriter("posting-" + this.wid + "-" + this.fileIndex + ".txt", "utf-8");
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            // write page length to file
            lengthWriter.println(docId + "," + wordCount);
        }

        // Process last chunk
        Collections.sort(res);
        for (Posting po : res) {
            postingWriter.println(po.toString());
        }
        postingWriter.close();
    }

    private boolean isAscii(String s) {
        return s.matches("\\A\\p{ASCII}*\\z");
    }

    private synchronized Integer getTermId(String word) {
        if (!termMap.containsKey(word)) {
            termMap.put(word, termId);
            termIdWriter.println(termId + "," + word);
            termId++;
        }
        return termMap.get(word);
    }

}
