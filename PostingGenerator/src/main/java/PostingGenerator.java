import java.io.*;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;

import javafx.print.Printer;
import org.jwat.common.HeaderLine;
import org.jwat.warc.WarcReader;
import org.jwat.warc.WarcReaderFactory;
import org.jwat.warc.WarcRecord;
import org.jsoup.*;
import org.jsoup.nodes.*;

public class PostingGenerator {

//    static String[] warcFiles =
//            {"/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00000.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00001.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00002.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00003.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00004.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00005.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00006.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00007.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00008.warc.gz",
//             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00009.warc.gz"};

    static String[] warcFiles =
            {"/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00000.warc.gz"};

    private static HashMap<String, Integer> urlMap = new HashMap<>();
    private static HashMap<String, Integer> termMap = new HashMap<>();
    private static int DOCID = 0;
    private static int TERMID = 0;
    private static PrintWriter docIdWriter;
    private static PrintWriter termIdWriter;

    public static void main(String[] args) {
        try {
            docIdWriter = new PrintWriter("page_table.txt", "utf-8");
            termIdWriter = new PrintWriter("term_table.txt", "utf-8");
        }
        catch (Exception e) {
            e.printStackTrace();
            return;
        }

        for (int i = 0; i < warcFiles.length; i++) {
            String warcFile = warcFiles[i];
            File file = new File(warcFile);
            try {
                System.out.println("processing " + warcFile);
                InputStream in = new FileInputStream(file);

                WarcReader reader = WarcReaderFactory.getReader(in);
                WarcRecord record;

                PrintWriter writer = new PrintWriter("posting" + Integer.toString(i) + ".txt", "utf-8");

                int index = 0;
                while ((record = reader.getNextRecord()) != null) {
                    // Only second record contains html content
                    index++;
                    if (index % 3 != 0) {
                        continue;
                    }

                    // extract url
                    String url = record.header.warcTargetUriStr;
                    if (url == null) {
                        continue;
                    }

                    // assign docId
                    int docId = getDocId(url);
                    System.out.println("processing doc " + docId);

                    String[] words = parseRecord(record, url);
                    if (words == null) {
                        continue;
                    }
                    HashMap<Integer, Integer> freq = new HashMap<>();
                    for (String w : words) {
                        if (isAscii(w)) {
                            int termId = getTermId(w);
                            freq.put(termId, freq.getOrDefault(termId, 0) + 1);
                        }
                    }

                    for (Integer ti : freq.keySet()) {
                        writer.println(ti + "," + docId + "," + freq.get(ti));
                    }
                }

                writer.close();
                System.out.println("--------------");
                reader.close();
                in.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        docIdWriter.close();
        termIdWriter.close();
    }

    // Load all from an input stream and produce a String
    private static String inputStreamToString(InputStream is) {
        ByteArrayOutputStream result = new ByteArrayOutputStream();

        byte[] buffer = new byte[1024];
        int length = 0;
        while (length != -1) {
            try {
                length = is.read(buffer);
                if (length != -1) {
                    result.write(buffer, 0, length);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        // StandardCharsets.UTF_8.name() > JDK 7
        String res = "";
        try {
            res = result.toString("UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return res;
    }

    private static int getDocId(String url) {
        if (!urlMap.containsKey(url)) {
            urlMap.put(url, DOCID);
            docIdWriter.println(DOCID + "," + url);
            DOCID++;
        }
        return urlMap.get(url);
    }

    private static int getTermId(String word) {
        if (!termMap.containsKey(word)) {
            termMap.put(word, TERMID);
            termIdWriter.println(TERMID + "," + word);
            TERMID++;
        }
        return termMap.get(word);
    }

    // parse a WARC record into an array of words
    private static String[] parseRecord(WarcRecord record, String url) {
        String[] words = null;
        try {
            InputStream is = record.getPayloadContent();
            String html = inputStreamToString(is);
            is.close();
            String doc = Jsoup.parse(html, url).body().text();
            words = doc.split("\\W+");
        }
        catch (Exception e) {
        }
        finally {
            return words;
        }
    }

    private static boolean isAscii(String s) {
        return s.matches("\\A\\p{ASCII}*\\z");
    }
}
