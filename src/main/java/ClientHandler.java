import java.io.*;
import java.net.Socket;

public class ClientHandler extends Thread {
    private final String GET_ALL_BOOKS = "GET_ALL_BOOKS";
    private final String GET = "GET";
    private final String BOOKS_Path = "C:\\Users\\OHtuz\\OneDrive\\Desktop\\ReaderHTTPserver\\Books\\";

    private final Socket client;
    private final BufferedReader in;
    private FileInputStream fin;
    private DataOutputStream dout;
    private BufferedWriter out;

    public ClientHandler(Socket client) {
        try {
            this.client = client;

            in = new BufferedReader(new InputStreamReader(client.getInputStream()));
            out = new BufferedWriter(new OutputStreamWriter(client.getOutputStream()));
            dout = new DataOutputStream(client.getOutputStream());
            start();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public void run() {
        try {
            String request, line, bookname;

            while (!client.isClosed()) {
                request = in.readLine();

                switch (request) {
                    case "GET-ALL-BOOK-NAMES" -> {
                        BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(BOOKS_Path + "AllBooks.txt")));
                        while ((line = br.readLine()) != null) {
                            out.write(line + "\n");
                            out.flush();
                        }
                        out.write("BOOK-NAMES-END\n");
                        out.flush();
                    }
                    case "GET-ABOUT" -> {
                        bookname = in.readLine();
                        BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(BOOKS_Path + bookname + ".txt")));
                        while ((line = br.readLine()) != null) {
                            out.write(line + "\n");
                            out.flush();
                        }
                    }
                    case "GET-PDF" -> {
                        bookname = in.readLine();
                        int bytes;

                        File file = new File(BOOKS_Path + bookname + ".pdf");
                        FileInputStream fileInputStream = new FileInputStream(file);

                        DataOutputStream dataOutputStream = new DataOutputStream(client.getOutputStream());
                        dataOutputStream.writeLong(file.length());

                        byte[] buffer = new byte[4 * 1024];
                        while ((bytes = fileInputStream.read(buffer)) != -1) {
                            dataOutputStream.write(buffer, 0, bytes);
                            dataOutputStream.flush();
                        }

                        fileInputStream.close();
                        dataOutputStream.close();
                    }
                }
            }

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
