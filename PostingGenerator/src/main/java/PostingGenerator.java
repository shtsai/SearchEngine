import java.io.Reader;

public class PostingGenerator {

    static String[] warcFiles =
            {"/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00000.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00001.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00002.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00003.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00004.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00005.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00006.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00007.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00008.warc.wet.gz",
             "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00009.warc.wet.gz"};

//    static String[] warcFiles =
//            {"/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00000.warc.wet.gz",
//            "/Users/Sean/Desktop/InvertedIndex/pages/CC-MAIN-20180918130631-20180918150631-00001.warc.wet.gz"};

    private static final int POSTING_SIZE = 1000000;
    private static final int READER_NUM = 2;
    private static final int WORKER_NUM = 10;

    public static void main(String[] args) {
        ReaderManager rm = new ReaderManager(warcFiles, READER_NUM);

        for (int i = 0; i < WORKER_NUM; i++) {
            Worker w = new Worker(rm, i, POSTING_SIZE);
            w.start();
        }
    }
}
