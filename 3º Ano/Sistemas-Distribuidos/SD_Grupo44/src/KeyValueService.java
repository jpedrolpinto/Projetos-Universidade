import java.io.*;
import java.util.*;
import java.net.SocketException;

public class KeyValueService {
    private final DataInputStream input;  // Input stream to read data from the server
    private final DataOutputStream output;  // Output stream to send data to the server

    // Constructor to initialize the input and output streams
    public KeyValueService(DataInputStream input, DataOutputStream output) {
        this.input = input;
        this.output = output;
    }

    // Method to send a "put" request to the server and store a key-value pair
    public void put(String key, byte[] value) throws IOException {
        try {
            // Send "put" command to the server
            output.writeUTF("put");
            output.writeUTF(key);  // Send the key
            output.writeInt(value.length);  // Send the length of the value
            output.write(value);  // Send the value itself

            // Read and print the server's response
            String response = input.readUTF();
            System.out.println(response);
        } catch (SocketException e) {
            throw new IOException("Connection lost during put operation", e);  // Handle connection errors
        }
    }

    // Method to retrieve a value for a specific key from the server
    public byte[] get(String key) throws IOException {
        output.writeUTF("get");  // Send "get" command to the server
        output.writeUTF(key);  // Send the key to retrieve

        // Read the server's response
        String response = input.readUTF();

        // If the key is found, read and return its value
        if ("Get_Ok".equals(response)) {
            int length = input.readInt();  // Get the length of the value
            byte[] value = new byte[length];  // Create a byte array to hold the value
            input.readFully(value);  // Read the value into the byte array
            return value;
        } else if ("Get_Not_Found".equals(response)) {
            System.out.println("Key not found: " + key);
            return null;
        }

        throw new IOException("Unknown server response: " + response);  // Handle unexpected responses
    }

    // Method to send multiple key-value pairs in a single "multiput" request
    public void multiPut(Map<String, byte[]> pairs) throws IOException {
        output.writeUTF("multiput");  // Send "multiput" command to the server
        output.writeInt(pairs.size());  // Send the number of key-value pairs

        // Send each key-value pair to the server
        for (Map.Entry<String, byte[]> entry : pairs.entrySet()) {
            output.writeUTF(entry.getKey());  // Send the key
            byte[] value = entry.getValue();  // Get the value
            output.writeInt(value.length);  // Send the length of the value
            output.write(value);  // Send the value
        }

        // Read and print the server's response
        System.out.println(input.readUTF());
    }

    // Method to send a "multiget" request to retrieve multiple keys
    public Map<String, byte[]> multiGet(List<String> keys) throws IOException {
        Map<String, byte[]> results = new HashMap<>();
        output.writeUTF("multiget");  // Send "multiget" command to the server
        output.writeInt(keys.size());  // Send the number of keys to retrieve

        // Send each key to the server
        keys.forEach(key -> {
            try {
                output.writeUTF(key);  // Send the key
            } catch (IOException e) {
                throw new RuntimeException(e);  // Handle any I/O exceptions
            }
        });

        int keyCount = input.readInt();  // Read the number of results returned by the server

        // For each key returned by the server, retrieve its value (if available)
        for (int i = 0; i < keyCount; i++) {
            String key = input.readUTF();  // Read the key
            int length = input.readInt();  // Read the length of the value

            if (length != -1) {
                byte[] value = new byte[length];  // Create a byte array for the value
                input.readFully(value);  // Read the value into the array
                results.put(key, value);  // Store the result
            } else {
                results.put(key, null);  // If the key wasn't found, store a null value
            }
        }
        return results;  // Return the map of key-value pairs
    }

    // Method to send a "getwhen" request to the server to wait for a condition to be met
    public byte[] getWhen(String key, String keyCond, byte[] valueCond) throws IOException {
        output.writeUTF("getwhen");  // Send "getwhen" command to the server
        output.writeUTF(key);  // Send the key to retrieve
        output.writeUTF(keyCond);  // Send the condition key
        output.writeInt(valueCond.length);  // Send the length of the condition value
        output.write(valueCond);  // Send the condition value
        output.flush();  // Ensure the data is sent immediately

        // Read the server's response
        String response = input.readUTF();

        // Handle the response based on its content
        return handleGetWhenResponse(response, key);
    }

    // Helper method to process the server's response to a "getwhen" request
    private byte[] handleGetWhenResponse(String response, String key) throws IOException {
        switch (response) {
            case "GetWhen_Ok":
                int length = input.readInt();  // Read the length of the value
                byte[] value = new byte[length];  // Create a byte array for the value
                input.readFully(value);  // Read the value into the array
                return value;  // Return the value

            case "GetWhen_Not_Found":
                System.out.println("Key not found: " + key);
                return null;  // Return null if the key wasn't found

            case "GetWhen_Timeout":
                System.out.println("Request timed out after 60 seconds");
                return null;  // Return null if the request timed out

            default:
                throw new IOException("Unknown server response: " + response);  // Handle unknown responses
        }
    }
}
