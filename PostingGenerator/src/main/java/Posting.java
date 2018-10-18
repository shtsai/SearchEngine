public class Posting implements Comparable<Posting> {
    private int termId;
    private int docId;
    private int freq;

    public Posting(int termId, int docId, int freq) {
        this.termId = termId;
        this.docId = docId;
        this.freq = freq;
    }

    @Override
    public int compareTo(Posting o) {
        if (this.termId != o.termId) {
            return this.termId - o.termId;
        } else {
            return this.docId - o.docId;
        }
    }

    @Override
    public String toString() {
        return termId + "," + docId + "," + freq;
    }
}
