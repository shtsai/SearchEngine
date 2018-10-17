import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class IndexBuilder {
    private static PrintWriter invertedListWriter;
    private static PrintWriter lexiconWriter;

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("ERROR: Missing input posting file");
            return;
        }

        // open write file
        try {
            invertedListWriter= new PrintWriter("inverted_list.txt", "utf-8");
            lexiconWriter = new PrintWriter("lexicon.txt", "utf-8");
        }
        catch (Exception e) {
            e.printStackTrace();
            return;
        }

        int curTermId = -1;
        int curOffset = 0;
        int curLength = 0;
        int curStartBlockIndex = 0;
        int curBlockIndex = 0;
        int prevDocId = 0;
        List<Integer> docIds = new ArrayList<>();
        List<Integer> freqs = new ArrayList<>();

        String postingsFile = args[0];
        File file = new File(postingsFile);
        FileReader fr;
        BufferedReader br;

        try {
            br = new BufferedReader(new FileReader(file));

            String st;
            while ((st = br.readLine()) != null) {

                String[] posting = st.split(",");
                int termId = Integer.parseInt(posting[0]);
                int docId = Integer.parseInt(posting[1]);
                int freq = Integer.parseInt(posting[2]);

                if (termId != curTermId) {
                    if (curTermId != -1) {
                        lexiconWriter.println(curTermId + "," + curStartBlockIndex + "," + curOffset + "," + curLength);
                    }
                    curStartBlockIndex = curBlockIndex;
                    curOffset = docIds.size();
                    curTermId = termId;
                    curLength = 0;
                    prevDocId = 0;
                }

                docIds.add(docId - prevDocId);
                prevDocId = docId;
                freqs.add(freq);
                curLength++;

                // a block is filled
                if (docIds.size() == 128) {
                    compressWrite(docIds, freqs);

                    curBlockIndex++;
                    docIds = new ArrayList<>();
                    freqs = new ArrayList<>();
                }

            }
        }
        catch (FileNotFoundException e) {
            System.out.println("ERROR: File not found");
            e.printStackTrace();
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // write entry for last term
        if (curTermId != -1) {
            lexiconWriter.println(curTermId + "," + curBlockIndex + "," + curOffset + "," + curLength);
        }

        lexiconWriter.close();
        invertedListWriter.close();
    }

    // perform vbyte compression on input number
    private static char[] vbyteCompress(int num) {
        int threshold = 128;
        List<Character> res = new ArrayList<>();
        while (num > 0) {
            char lower = (char) (num % threshold);
            num /= threshold;
            res.add(lower);
        }
        int len = res.size();
        char[] array = new char[len];
        for (int i = 0; i < len; i++) {
            array[i] = res.get(len - 1 - i);
            if (i + 1 < len) {  // signal more coming
                array[i] |= (1 << 8);
            }
        }

        return array;
    }

    // convert a character to a string in binary format
    private static String charToBinary(char c) {
        StringBuilder sb = new StringBuilder();
        for (int i = 7; i >= 0; i--) {
            int bit =  c & (1 << i);
            sb.append(bit == 0 ? "0" : "1");
        }
        return sb.toString();
    }

    private static void compressWrite(List<Integer> docIds, List<Integer> freqs) {
        List<Character> docIdsCompressed = new ArrayList<>();
        List<Character> freqsCompressed = new ArrayList<>();
        for (Integer di : docIds) {
            char[] vcs = vbyteCompress(di);
            for (char vc : vcs) {
                docIdsCompressed.add(vc);
            }
        }
        for (Integer f : freqs) {
            char[] vcs = vbyteCompress(f);
            for (char vc : vcs) {
                freqsCompressed.add(vc);
            }
        }
        int size = docIdsCompressed.size() + freqsCompressed.size();
        for (char s : vbyteCompress(size)) {
            invertedListWriter.print(s);
        }
        for (char di : docIdsCompressed) {
            invertedListWriter.print(di);
        }
        for (char f : freqsCompressed) {
            invertedListWriter.print(f);
        }
    }
}

