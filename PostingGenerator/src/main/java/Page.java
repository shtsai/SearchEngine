import org.jsoup.Jsoup;
import org.jwat.warc.WarcRecord;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;

public class Page {
    private int docId;
    private String html;

    public Page(int docId, String url, WarcRecord record) {
        this.docId = docId;
        this.html = parseHtml(record);
    }

    public int getDocId() {
        return docId;
    }

    public String getHtml() {
        return html;
    }

    private String parseHtml(WarcRecord record) {
        String res = null;
        try {
            InputStream is = record.getPayloadContent();
            res = inputStreamToString(is);
            is.close();
        }
        catch (Exception e) {
        }
        finally {
            return res;
        }
    }

    // Load all from an input stream and produce a String
    private String inputStreamToString(InputStream is) {
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
}
