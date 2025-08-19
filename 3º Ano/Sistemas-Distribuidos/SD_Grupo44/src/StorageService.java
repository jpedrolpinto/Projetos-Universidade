import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.ReentrantLock;
import java.util.List;
import java.util.HashMap;

public class StorageService {
    // A thread-safe map to store key-value pairs, using ConcurrentHashMap to ensure thread safety
    private final Map<String, byte[]> store = new ConcurrentHashMap<>();
    
    // Reentrant lock to ensure that multiple threads do not concurrently modify the map
    private final ReentrantLock storeLock = new ReentrantLock();

    // Method to store a key-value pair in the map
    public void put(String key, byte[] value) {
        storeLock.lock();  // Acquire the lock to ensure thread-safety during modification
        try {
            store.put(key, value);  // Put the key-value pair in the store
        } finally {
            storeLock.unlock();  // Ensure that the lock is released after the operation
        }
    }

    // Method to retrieve the value for a specific key from the map
    public byte[] get(String key) {
        storeLock.lock();  // Acquire the lock to ensure thread-safety during retrieval
        try {
            return store.get(key);  // Return the value associated with the key
        } finally {
            storeLock.unlock();  // Ensure that the lock is released after the operation
        }
    }

    // Method to store multiple key-value pairs in the map
    public void multiPut(Map<String, byte[]> pairs) {
        storeLock.lock();  // Acquire the lock to ensure thread-safety during modification
        try {
            store.putAll(pairs);  // Put all the key-value pairs in the store
        } finally {
            storeLock.unlock();  // Ensure that the lock is released after the operation
        }
    }

    // Method to retrieve the values for a list of keys from the map
    public Map<String, byte[]> multiGet(List<String> keys) {
        storeLock.lock();  // Acquire the lock to ensure thread-safety during retrieval
        try {
            Map<String, byte[]> results = new HashMap<>();  // Create a map to hold the results
            for (String key : keys) {
                byte[] value = store.get(key);  // Retrieve the value for each key
                results.put(key, value);  // Put the key-value pair in the results map
            }
            return results;  // Return the map of key-value pairs
        } finally {
            storeLock.unlock();  // Ensure that the lock is released after the operation
        }
    }

    // Method to retrieve the value for a specific key (this is a simplified version of the get method)
    public byte[] getValue(String key) {
        storeLock.lock();  // Acquire the lock to ensure thread-safety during retrieval
        try {
            return store.get(key);  // Return the value associated with the key
        } finally {
            storeLock.unlock();  // Ensure that the lock is released after the operation
        }
    }
}
