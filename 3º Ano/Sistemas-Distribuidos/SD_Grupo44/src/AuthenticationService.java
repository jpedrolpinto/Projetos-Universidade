import java.io.*;
import java.util.Scanner;

public class AuthenticationService {
    private final DataInputStream input;  // Input stream to read data from the client
    private final DataOutputStream output; // Output stream to send data to the client

    // Constructor to initialize the input and output streams for communication
    public AuthenticationService(DataInputStream input, DataOutputStream output) {
        this.input = input;
        this.output = output;
    }

    // Method to authenticate or register a user based on the input choice
    public boolean authenticateOrRegister(Scanner scanner) throws IOException {
        String prompt = input.readUTF();  // Receive a prompt from the server, e.g., "Choose 1 for Login, 2 for Register"
        System.out.println(prompt);
        
        String choice = scanner.nextLine();  // Read the user's choice (login or register)
        output.writeUTF(choice);  // Send the choice to the server
        
        if ("1".equals(choice)) {  // If the user chooses login
            return handleLogin(scanner);
        } else if ("2".equals(choice)) {  // If the user chooses registration
            return handleRegistration(scanner);
        } else {  // Invalid choice
            System.out.println("Invalid choice.");
            return false;
        }
    }

    // Method to handle login process
    private boolean handleLogin(Scanner scanner) throws IOException {
        // Prompt for username and password
        System.out.println(input.readUTF());  // "Enter username:"
        String username = scanner.nextLine();
        output.writeUTF(username);  // Send the username to the server

        System.out.println(input.readUTF());  // "Enter password:"
        String password = scanner.nextLine();
        output.writeUTF(password);  // Send the password to the server

        // Get the server's response
        String response = input.readUTF();
        System.out.println(response);

        // Check the server's response to determine if login was successful
        if (response.contains("successful")) {
            return true;  // Login was successful
        } else if (response.contains("does not exist")) {  // If the user does not exist
            return handleNonExistentUser(scanner);
        } else if (response.contains("Incorrect password")) {  // If the password is incorrect
            System.out.println("Incorrect password. Would you like to try again?");
            String choice = scanner.nextLine();
            output.writeUTF(choice);  // Send the user's decision back to the server

            // If the user chooses to try again, prompt for password
            if ("1".equals(choice)) {
                return handleLogin(scanner);  // Recursively call handleLogin to retry the login process
            }
        }
        return false;  // Return false if login fails
    }

    // Method to handle the situation when the user does not exist in the system
    private boolean handleNonExistentUser(Scanner scanner) throws IOException {
        System.out.println("Choose: 1-Try Again, 2-Register");
        String choice = scanner.nextLine();  // Ask the user to either try again or register
        output.writeUTF(choice);  // Send the user's choice to the server
        return "2".equals(choice) && handleRegistration(scanner);  // If the user chooses to register, proceed with registration
    }

    // Method to handle user registration
    private boolean handleRegistration(Scanner scanner) throws IOException {
        // Prompt for new username and password
        System.out.println(input.readUTF());  // "Enter new username:"
        String username = scanner.nextLine();
        output.writeUTF(username);  // Send the new username to the server

        System.out.println(input.readUTF());  // "Enter new password:"
        String password = scanner.nextLine();
        output.writeUTF(password);  // Send the new password to the server

        // Get the server's response
        String response = input.readUTF();
        System.out.println(response);

        // Check the server's response to determine if registration was successful
        return response.contains("successful") || response.contains("sucedido");  // If successful in English or Spanish
    }
}
