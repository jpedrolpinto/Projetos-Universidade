import java.util.*;
import java.io.IOException;

public class CommandLineInterface {
    private static final String HELP_MESSAGE = """
        Available commands:
        put key=value  - Add a key-value pair
        get key        - Get value for a key
        multiput key1=value1 key2=value2 ... - Add multiple key-value pairs
        multiget key1 key2 ... - Get values for multiple keys
        getwhen key keycond keyvalue - Wait for keycond to be keyvalue and return key
        exit           - Close connection
        help           - Show this help message
        """;
 
    private final KeyValueService keyValueService;
    private final Scanner scanner;
 
    public CommandLineInterface(KeyValueService keyValueService, Scanner scanner) {
        this.keyValueService = keyValueService;
        this.scanner = scanner;
    }
 
    public void start() {
        while (true) {
            try {
                System.out.print("> ");
                String[] parts = scanner.nextLine().split("\\s+");
                if (!processCommand(parts[0].toLowerCase(), parts)) {
                    break;
                }
            } catch (Exception e) {
                System.err.println("Error: " + e.getMessage());
            }
        }
    }
 
    private boolean processCommand(String command, String[] parts) throws IOException, InterruptedException {
        return switch (command) {
            case "help" -> { System.out.println(HELP_MESSAGE); yield true; }
            case "exit" -> false;
            case "put" -> { handlePut(parts); yield true; }
            case "get" -> { handleGet(parts); yield true; }
            case "multiput" -> { handleMultiPut(parts); yield true; }
            case "multiget" -> { handleMultiGet(parts); yield true; }
            case "getwhen" -> { handleGetWhen(parts); yield true; }
            default -> { System.out.println("Invalid command. Use 'help' for commands."); yield true; }
        };
    }
 
    private void handlePut(String[] parts) throws IOException, InterruptedException {
        if (parts.length == 2) {
            String[] keyValue = parts[1].split("=");
            if (keyValue.length == 2) {
                keyValueService.put(keyValue[0], keyValue[1].getBytes());
            } else {
                System.out.println("Usage: put key=value");
            }
        } else {
            System.out.println("Usage: put key=value");
        }
    }
 
    private void handleGet(String[] parts) throws IOException, InterruptedException {
        if (parts.length == 2) {
            byte[] value = keyValueService.get(parts[1]);
            if (value != null) {
                System.out.println("Value: " + new String(value));
            }
        } else {
            System.out.println("Usage: get key");
        }
    }
 
    private void handleMultiPut(String[] parts) throws IOException, InterruptedException {
        Map<String, byte[]> pairs = new HashMap<>();
        for (int i = 1; i < parts.length; i++) {
            String[] keyValue = parts[i].split("=");
            if (keyValue.length == 2) {
                pairs.put(keyValue[0], keyValue[1].getBytes());
            }
        }
        if (!pairs.isEmpty()) {
            keyValueService.multiPut(pairs);
        } else {
            System.out.println("Usage: multiput key1=value1 key2=value2 ...");
        }
    }
 
    private void handleMultiGet(String[] parts) throws IOException, InterruptedException {
        if (parts.length > 1) {
            List<String> keys = Arrays.asList(Arrays.copyOfRange(parts, 1, parts.length));
            Map<String, byte[]> results = keyValueService.multiGet(keys);
            results.forEach((key, value) -> 
                System.out.println(key + ": " + (value != null ? new String(value) : "not found"))
            );
        } else {
            System.out.println("Usage: multiget key1 key2 ...");
        }
    }
 
    private void handleGetWhen(String[] parts) throws IOException, InterruptedException {
        if (parts.length == 4) {
            byte[] result = keyValueService.getWhen(parts[1], parts[2], parts[3].getBytes());
            if (result != null) {
                System.out.println("Value: " + new String(result));
            }
        } else {
            System.out.println("Usage: getwhen key keyCond valueCond");
        }
    }
 }