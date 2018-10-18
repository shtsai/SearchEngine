import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class ReaderManager {
    private BlockingQueue<Page> queue;
    private int remainingWorker;
    private HashMap<String, Integer> urlMap;

    public ReaderManager(String[] warcFiles, int readerNum) {
        queue = new LinkedBlockingQueue<>();
        remainingWorker = readerNum;
        urlMap = new HashMap<>();

        List<List<String>> assignments = divideReads(warcFiles, readerNum);

        for (int i = 0; i < readerNum; i++) {
            FileReader reader = new FileReader(this, assignments.get(i), i);
            reader.start();
        }
    }

    public BlockingQueue<Page> getQueue() {
        return queue;
    }

    public HashMap<String, Integer> getUrlMap() {
        return urlMap;
    }

    public boolean isFinished() {
        return remainingWorker == 0;
    }

    public synchronized void decrementWorker() {
        remainingWorker--;
    }

    private List<List<String>> divideReads(String[] files, int readerNum) {
        List<List<String>> assignments = new ArrayList<>();
        for (int i = 0; i < files.length; i++) {
            int index = i % readerNum;
            if (assignments.size() <= index) {
                assignments.add(new ArrayList<>());
            }
            assignments.get(index).add(files[i]);
        }
        return assignments;
    }

}
