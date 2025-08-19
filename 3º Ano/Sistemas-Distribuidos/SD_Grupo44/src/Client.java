import java.io.*;
import java.net.*;
import java.util.Scanner;

public class Client implements AutoCloseable {
    private final Socket socket;  // Socket for client-server communication
    private final DataInputStream input;  // Input stream to receive data from the server
    private final DataOutputStream output;  // Output stream to send data to the server
    private final AuthenticationService authService;  // Service for user authentication and registration
    private final KeyValueService kvService;  // Service for key-value operations
    private final CommandLineInterface cli;  // CLI interface to interact with the user

    // Constructor to initialize the client with the server's host and port
    public Client(String host, int port) throws IOException {
        this.socket = new Socket(host, port);  // Create a socket connection to the server
        this.input = new DataInputStream(socket.getInputStream());  // Initialize input stream for data from the server
        this.output = new DataOutputStream(socket.getOutputStream());  // Initialize output stream for data to the server
        this.authService = new AuthenticationService(input, output);  // Create an authentication service object
        this.kvService = new KeyValueService(input, output);  // Create a key-value service object
        this.cli = new CommandLineInterface(kvService, new Scanner(System.in));  // Initialize the command-line interface
    }

    // Method to start the client application
    public void start() throws IOException {
        Scanner scanner = new Scanner(System.in);  // Scanner to read user input
        // Try to authenticate or register the user
        if (!authService.authenticateOrRegister(scanner)) {
            System.out.println("Authentication failed. Exiting.");  // If authentication fails, exit the program
            return;
        }

        System.out.println("Authentication successful! Type 'help' for available commands:");
        cli.start();  // Start the command-line interface to interact with the key-value service
    }

    // Method to close the resources (socket, input/output streams)
    @Override
    public void close() throws IOException {
        if (output != null) {
            try {
                output.writeUTF("exit");  // Send an exit command to the server
                output.flush();  // Ensure the data is sent immediately
            } catch (IOException e) {
                System.err.println("Error sending exit command: " + e.getMessage());  // Handle any error sending the exit command
            }
        }
        
        if (input != null) input.close();  // Close the input stream
        if (output != null) output.close();  // Close the output stream
        if (socket != null && !socket.isClosed()) socket.close();  // Close the socket if it's not already closed
    }

    // Main method to start the client application
    public static void main(String[] args) {
        try (Client client = new Client("localhost", 11111)) {  // Try to create and start the client
            client.start();
        } catch (IOException e) {
            System.err.println("Error: " + e.getMessage());  // Catch and print any IOException
        }
    }
}
