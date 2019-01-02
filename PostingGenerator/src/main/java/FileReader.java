import java.io.*;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import javafx.print.Printer;
import org.jsoup.Jsoup;
import org.jwat.common.HeaderLine;
import org.jwat.warc.WarcReader;
import org.jwat.warc.WarcReaderFactory;
import org.jwat.warc.WarcRecord;

public class FileReader extends Thread {

    private static final String PAGE_TABLE = "run/page_table-";

    private ReaderManager rm;
    private List<String> files;
    private int rid;
    private BlockingQueue<Page> queue;
    private static int docId = 0;
    private HashMap<String, Integer> urlMap;
    private PrintWriter docIdWriter;

    public FileReader(ReaderManager rm, List<String> files, int rid) {
        super();
        this.rm = rm;
        this.files = files;
        this.rid = rid;
        this.queue = rm.getQueue();
        this.urlMap = rm.getUrlMap();

        try {
            docIdWriter = new PrintWriter(PAGE_TABLE + this.rid + ".txt", "utf-8");
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }


    public void run() {
        for (int i = 0; i < this.files.size(); i++) {
            String warcFile = this.files.get(i);
            File file = new File(warcFile);
            try {
                System.out.println("reader " + rid + " reading " + warcFile);
                InputStream in = new FileInputStream(file);

                System.out.println(in);
                
                WarcReader reader = WarcReaderFactory.getReader(in);
                WarcRecord record;

                int index = 0;
                while ((record = reader.getNextRecord()) != null) {
                    index++;

                    String url = record.header.warcTargetUriStr;
                    if (url == null) {
                        continue;
                    }

                    int did = generateDocId(url);
                    if (queue.size() > 5000) {
                        Thread.sleep(100);
                    }
                    queue.add(new Page(did, url, record));
                }
                reader.close();
                in.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }
            clearUrlMap();
        }

        this.rm.decrementWorker();
    }

    private synchronized int generateDocId(String url) {
        if (!urlMap.containsKey(url)) {
            urlMap.put(url, docId);
            docIdWriter.println(docId + "," + url);
            docId++;
        }
        return urlMap.get(url);
    }

    private synchronized void clearUrlMap() {
        urlMap = new HashMap<>();
    }
}
