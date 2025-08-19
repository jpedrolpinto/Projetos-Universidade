import java.io.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.locks.ReentrantLock;

public class GetWhenService {
    // Timeout period for waiting on a condition (in milliseconds)
    private static final long GETWHEN_TIMEOUT = 60000;
    // A queue to hold requests for the "GetWhen" service
    private final BlockingQueue<GetWhenRequest> requests = new LinkedBlockingQueue<>();
    // The storage service that provides methods to get values for keys
    private final StorageService storageService;
    // Lock to ensure thread safety when processing requests
    private final ReentrantLock getWhenLock = new ReentrantLock();

    // Constructor to initialize the GetWhenService with the given StorageService
    public GetWhenService(StorageService storageService) {
        this.storageService = storageService;
        startProcessingThread(); // Start the background thread to process requests
    }

    // Internal class to represent a "GetWhen" request
    static class GetWhenRequest {
        final String key;            // The key to retrieve when the condition is met
        final String keyCond;        // The condition key that is checked
        final byte[] valueCond;      // The value condition that must be met
        final long timestamp;        // The timestamp when the request was created
        final DataOutputStream clientOutput; // The client's output stream to send the response

        // Constructor to create a new GetWhenRequest
        GetWhenRequest(String key, String keyCond, byte[] valueCond, DataOutputStream clientOutput) {
            this.key = key;
            this.keyCond = keyCond;
            this.valueCond = valueCond;
            this.clientOutput = clientOutput;
            this.timestamp = System.currentTimeMillis(); // Record the current timestamp
        }

        // Check if the request has timed out
        boolean isTimedOut() {
            return System.currentTimeMillis() - timestamp > GETWHEN_TIMEOUT;
        }
    }

    // Method to add a new request to the queue
    public void addRequest(String key, String keyCond, byte[] valueCond, DataOutputStream clientOutput) {
        getWhenLock.lock();  // Lock to ensure thread safety when adding a request
        try {
            // Add the new request to the queue
            requests.offer(new GetWhenRequest(key, keyCond, valueCond, clientOutput));
        } finally {
            getWhenLock.unlock(); // Release the lock
        }
    }

    // Method to process a single request from the queue
    private void processRequest(GetWhenRequest request) throws IOException {
        getWhenLock.lock(); // Lock to ensure thread safety when processing a request
        try {
            // If the request has timed out, send a timeout message to the client
            if (request.isTimedOut()) {
                request.clientOutput.writeUTF("GetWhen_Timeout");
                return;
            }

            // Retrieve the current value for the condition key
            byte[] currentValue = storageService.getValue(request.keyCond);
            
            // Check if the current value matches the condition value
            if (currentValue != null && Arrays.equals(currentValue, request.valueCond)) {
                // If the condition is met, retrieve the value for the requested key
                byte[] result = storageService.getValue(request.key);
                if (result != null) {
                    // Send the value to the client if found
                    request.clientOutput.writeUTF("GetWhen_Ok");
                    request.clientOutput.writeInt(result.length); // Send the length of the value
                    request.clientOutput.write(result);           // Send the value
                } else {
                    // If the requested value is not found, notify the client
                    request.clientOutput.writeUTF("GetWhen_Not_Found");
                }
                request.clientOutput.flush(); // Ensure the output is sent to the client
            } else {
                // If the condition is not met, re-add the request to the queue for retry
                requests.offer(request);
                Thread.sleep(100); // Wait for a short time before checking again
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt(); // Handle interruption gracefully
        } finally {
            getWhenLock.unlock(); // Release the lock
        }
    }

    // Start a background thread that continuously processes requests from the queue
    private void startProcessingThread() {
        new Thread(() -> {
            while (true) {
                try {
                    // Take and process a request from the queue
                    processRequest(requests.take());
                } catch (Exception e) {
                    e.printStackTrace(); // Log any exceptions that occur during processing
                }
            }
        }).start();
    }
}
