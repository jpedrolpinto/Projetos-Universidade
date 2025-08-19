import java.io.*;
import java.net.*;
import java.util.concurrent.*;

public class Server {
    // Semaphore to limit the number of clients that can access the server at once
    private final Semaphore semaphore;

    // Instances of services: storage, user, and the getWhen service
    private final StorageService storageService;
    private final UserService userService;
    private final GetWhenService getWhenService;

    // Port where the server listens for client connections
    private final int port;

    // Constructor to initialize the server with maximum clients and port
    public Server(int maxClients, int port) {
        this.semaphore = new Semaphore(maxClients); // Initialize the semaphore with a limit on the number of concurrent clients
        this.storageService = new StorageService(); // Initialize the storage service
        this.userService = new UserService(); // Initialize the user service
        this.getWhenService = new GetWhenService(storageService); // Initialize the getWhen service, passing the storage service
        this.port = port; // Set the server port
    }

    // Method to start the server and handle incoming client connections
    public void start() {
        try (ServerSocket serverSocket = new ServerSocket(port)) { // Create a ServerSocket bound to the specified port
            System.out.println("Server running on port: " + port); // Print server information

            // Loop to continuously accept client connections
            while (true) {
                // Accept a client connection and create a socket for communication
                Socket clientSocket = serverSocket.accept();

                // Start a new thread to handle the client connection
                new Thread(new ClientHandler(
                    clientSocket,            // Client's socket
                    storageService,          // Storage service
                    userService,             // User service
                    getWhenService,          // GetWhen service
                    semaphore                // Semaphore to control concurrent access
                )).start(); // Start the thread that will handle the client
            }
        } catch (IOException e) { // Catch any IOException during the server's operation
            System.err.println("Server error: " + e.getMessage()); // Print any error that occurs
        }
    }

    // Main method to start the server with specified maximum clients and port
    public static void main(String[] args) {
        int maxClients = 5; // Maximum number of clients allowed to connect at once
        int port = 11111; // Port the server will listen on
        new Server(maxClients, port).start(); // Create a new server and start it
    }
}
