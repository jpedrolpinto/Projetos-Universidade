import java.io.*;
import java.net.Socket;
import java.util.Map;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.ReentrantLock;
import java.util.*;

public class ClientHandler implements Runnable {
    private final Socket socket;  // The socket through which the client communicates with the server
    private final StorageService storageService;  // Service that handles storage of key-value pairs
    private final UserService userService;  // Service that handles user authentication and registration
    private final GetWhenService getWhenService;  // Service for handling special conditions (like "getwhen")
    private final Semaphore semaphore;  // Semaphore to limit the number of clients that can be processed concurrently
    private final ReentrantLock clientLock = new ReentrantLock();  // Lock for handling concurrent client requests
    private DataInputStream input;  // Input stream for receiving data from the client
    private DataOutputStream output;  // Output stream for sending data to the client

    // Constructor that initializes the client handler with necessary services and socket
    public ClientHandler(Socket socket, 
                        StorageService storageService,
                        UserService userService, 
                        GetWhenService getWhenService,
                        Semaphore semaphore) {
        this.socket = socket;
        this.storageService = storageService;
        this.userService = userService;
        this.getWhenService = getWhenService;
        this.semaphore = semaphore;
    }

    // Main method of the thread that will be executed when the client is handled
    @Override
    public void run() {
        try {
            semaphore.acquire();  // Acquiring a semaphore to control access to the server

            setupStreams();  // Setting up input and output streams

            clientLock.lock();  // Locking the client to ensure only one request is handled at a time
            try {
                if (!handleAuthentication()) {  // Handling authentication
                    return;  // If authentication fails, terminate the client handling
                }
            } finally {
                clientLock.unlock();  // Unlocking the client after authentication
            }

            handleClientRequests();  // Handle client commands (put, get, multiput, etc.)

        } catch (IOException | InterruptedException e) {
            System.err.println("Error handling client: " + e.getMessage());  // Handling any IO or interrupt errors
        } finally {
            cleanup();  // Cleaning up resources after client handling
        }
    }

    // Handles the "put" command to store a key-value pair in the storage service
    private void handlePut() throws IOException {
        clientLock.lock();  // Locking the client to ensure no other request happens simultaneously
        try {
            String key = input.readUTF();  // Reading the key from the client
            int length = input.readInt();  // Reading the length of the value
            byte[] value = new byte[length];
            input.readFully(value);  // Reading the value from the client
            
            storageService.put(key, value);  // Storing the key-value pair
            output.writeUTF("Put_Ok");  // Sending a response to the client
        } finally {
            clientLock.unlock();  // Unlocking the client after handling the request
        }
    }

    // Handles the "get" command to retrieve a value based on the key from the storage service
    private void handleGet() throws IOException {
        clientLock.lock();  // Locking the client to avoid concurrent access
        try {
            String key = input.readUTF();  // Reading the key from the client
            byte[] value = storageService.get(key);  // Retrieving the value from storage
            
            if (value != null) {
                output.writeUTF("Get_Ok");  // Sending a success message to the client
                output.writeInt(value.length);  // Sending the length of the value
                output.write(value);  // Sending the value to the client
            } else {
                output.writeUTF("Get_Not_Found");  // Sending a failure message if the key is not found
            }
        } finally {
            clientLock.unlock();  // Unlocking after handling the request
        }
    }

    // Handles the "multiput" command to store multiple key-value pairs
    private void handleMultiPut() throws IOException {
        clientLock.lock();  // Locking the client
        try {
            int numPairs = input.readInt();  // Reading the number of key-value pairs
            Map<String, byte[]> pairs = new HashMap<>();
            
            for (int i = 0; i < numPairs; i++) {
                String key = input.readUTF();  // Reading each key
                int length = input.readInt();  // Reading the length of the value
                byte[] value = new byte[length];
                input.readFully(value);  // Reading the value
                pairs.put(key, value);  // Storing the key-value pair in the map
            }
            
            storageService.multiPut(pairs);  // Storing the multiple pairs
            output.writeUTF("MultiPut_Ok");  // Sending a success response
        } finally {
            clientLock.unlock();  // Unlocking after handling the request
        }
    }

    // Setup the input and output streams for communication with the client
    private void setupStreams() throws IOException {
        input = new DataInputStream(socket.getInputStream());  // Initialize input stream
        output = new DataOutputStream(socket.getOutputStream());  // Initialize output stream
    }

    // Handle authentication (login or registration)
    private boolean handleAuthentication() throws IOException {
        output.writeUTF("1-Login, 2-Register");  // Sending authentication prompt to the client
        
        String choice = input.readUTF();  // Reading the user's choice
        return "1".equals(choice) ? handleLogin() : handleRegistration();  // Handle login or registration based on the choice
    }

    // Handle the login process
    private boolean handleLogin() throws IOException {
        output.writeUTF("Enter username:");  // Prompt for username
        String username = input.readUTF();  // Read the username
        
        output.writeUTF("Enter password:");  // Prompt for password
        String password = input.readUTF();  // Read the password

        if (userService.authenticate(username, password)) {
            output.writeUTF("Login successful!");  // If successful, send success message
            return true;
        } else {
            output.writeUTF("Login failed!");  // If failed, send failure message
            return false;
        }
    }

    // Handle the registration process
    private boolean handleRegistration() throws IOException {
        output.writeUTF("Enter new username:");  // Prompt for new username
        String username = input.readUTF();  // Read the username
        
        output.writeUTF("Enter new password:");  // Prompt for new password
        String password = input.readUTF();  // Read the password

        try {
            userService.saveUser(username, password);  // Save the new user in the service
            output.writeUTF("Registration successful!");  // If successful, send confirmation
            return true;
        } catch (IOException e) {
            output.writeUTF("Registration failed: " + e.getMessage());  // If an error occurs, send failure message
            return false;
        }
    }

    // Handle different client commands like "put", "get", "multiput", etc.
    private void handleClientRequests() throws IOException {
        while (true) {
            String command = input.readUTF();  // Read the command from the client
            
            switch (command) {
                case "exit" -> {
                    return;  // Exit the handler when "exit" is received
                }
                case "put" -> handlePut();  // Handle "put" command
                case "get" -> handleGet();  // Handle "get" command
                case "multiput" -> handleMultiPut();  // Handle "multiput" command
                case "multiget" -> handleMultiGet();  // Handle "multiget" command
                case "getwhen" -> handleGetWhen();  // Handle "getwhen" command
                default -> output.writeUTF("Unknown command");  // Handle unknown commands
            }
        }
    }

    // Handles the "multiget" command to retrieve multiple key-value pairs
    private void handleMultiGet() throws IOException {
        clientLock.lock();  // Locking the client to avoid concurrency issues
        try {
            int numKeys = input.readInt();  // Reading the number of keys
            output.writeInt(numKeys);  // Sending the number of keys back to the client
            
            List<String> keys = new ArrayList<>(numKeys);
            
            // Reading all keys from the client
            for (int i = 0; i < numKeys; i++) {
                String key = input.readUTF();
                keys.add(key);
            }
 
            // Retrieving all key-value pairs using the multiGet method from storage service
            Map<String, byte[]> results = storageService.multiGet(keys);
    
            // Sending the results back to the client
            for (String key : keys) {
                byte[] value = results.get(key);
                output.writeUTF(key);  // Sending the key
                
                if (value != null) {
                    output.writeInt(value.length);  // Sending the length of the value
                    output.write(value);  // Sending the value
                } else {
                    output.writeInt(-1);  // If no value was found, send -1
                }
            }
        } finally {
            clientLock.unlock();  // Unlocking the client
        }
    }

    // Handles the "getwhen" command (for special conditions on retrieval)
    private void handleGetWhen() throws IOException {
        String key = input.readUTF();  // Reading the key
        String keyCond = input.readUTF();  // Reading the condition key
        int length = input.readInt();  // Reading the length of the condition value
        byte[] valueCond = new byte[length];
        input.readFully(valueCond);  // Reading the condition value
        
        getWhenService.addRequest(key, keyCond, valueCond, output);  // Adding the request to the "getwhen" service
    }

    // Cleaning up resources after client handling
    private void cleanup() {
        try {
            semaphore.release();  // Releasing the semaphore to allow other clients
            if (input != null) input.close();  // Closing the input stream
            if (output != null) output.close();  // Closing the output stream
            if (socket != null) socket.close();  // Closing the socket connection
        } catch (IOException e) {
            System.err.println("Error during cleanup: " + e.getMessage());  // Handling any cleanup errors
        }
    }
}
