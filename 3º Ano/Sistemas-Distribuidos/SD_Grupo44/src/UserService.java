import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

public class UserService {
    // A thread-safe map to store usernames and their corresponding passwords
    private final Map<String, String> userDatabase = new ConcurrentHashMap<>();
    private static final String USER_CSV_FILE = "src/users.csv";  // File that stores user data in CSV format

    // Constructor initializes the user service and loads the user data from the CSV file
    public UserService() {
        loadUserDatabase();  // Load user database from file upon initialization
    }

    // Loads the user database from the CSV file (if it exists)
    private void loadUserDatabase() {
        try {
            // Check if the file exists, if not, create an empty file
            if (!Files.exists(Paths.get(USER_CSV_FILE))) {
                Files.createFile(Paths.get(USER_CSV_FILE));
                return;  // Exit if the file doesn't exist
            }

            // Read all lines from the CSV file
            List<String> lines = Files.readAllLines(Paths.get(USER_CSV_FILE));
            for (String line : lines) {
                // Split each line by the comma to extract the username and password
                String[] parts = line.split(",");
                if (parts.length == 2) {
                    // Add the username and password to the userDatabase map
                    userDatabase.put(parts[0], parts[1]);
                }
            }
            System.out.println("User database loaded. Total users: " + userDatabase.size());
        } catch (IOException e) {
            // If an error occurs while reading the file, print the error message
            System.err.println("Error loading user database: " + e.getMessage());
        }
    }

    // Registers a new user and saves their username and password to the database and CSV file
    public synchronized void saveUser(String username, String password) throws IOException {
        // Check if the username already exists in the database
        if (userDatabase.containsKey(username)) {
            throw new IOException("Username already exists");  // If exists, throw an exception
        }

        // Add the new user to the userDatabase map
        userDatabase.put(username, password);

        // Prepare the user entry to write to the CSV file (username,password)
        String userEntry = username + "," + password + "\n";

        // Append the user entry to the CSV file
        Files.write(Paths.get(USER_CSV_FILE), 
                   userEntry.getBytes(), 
                   StandardOpenOption.CREATE, 
                   StandardOpenOption.APPEND);
        System.out.println("User registered: " + username);
    }

    // Authenticates a user by checking if the username exists and if the password matches
    public boolean authenticate(String username, String password) {
        return userDatabase.containsKey(username) && 
               userDatabase.get(username).equals(password);  // Check if username and password match
    }

    // Checks if the user already exists in the database
    public boolean userExists(String username) {
        return userDatabase.containsKey(username);  // Returns true if the username exists
    }
}
