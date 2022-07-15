import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;


public class Server {
    private ServerSocket server;

    public Server() {
        try {
            server = new ServerSocket(40002, 50);
            while (true) {
                Socket client = server.accept();
                new ClientHandler(client);
            }
        } catch (IOException ignored) {
            ignored.printStackTrace();
        }
    }
}
