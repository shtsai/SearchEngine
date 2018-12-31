import java.util.Collections;
import java.io.Reader;
import java.io.File;
import java.util.ArrayList;

public class PostingGenerator {

    private static final String CWD = System.getProperty("user.dir");
    private static final String DIR = CWD + "/pages";
    private static final int POSTING_SIZE = 1000000;
    private static final int READER_NUM = 2;
    private static final int WORKER_NUM = 10;

    public static void main(String[] args) {
        // read all WARC file in the pages/ folder
        File folder = new File(DIR);
        File[] files = folder.listFiles();
        ArrayList<String> warcFiles = new ArrayList<>();
        for (File f : files) {
          warcFiles.add(DIR + "/" + f.getName());
        }
        Collections.sort(warcFiles);

        ReaderManager rm = new ReaderManager(warcFiles, READER_NUM);

        for (int i = 0; i < WORKER_NUM; i++) {
            Worker w = new Worker(rm, i, POSTING_SIZE);
            w.start();
        }
    }
}
