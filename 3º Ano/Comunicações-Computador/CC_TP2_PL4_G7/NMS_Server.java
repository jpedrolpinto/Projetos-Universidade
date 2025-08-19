import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.reflect.TypeToken;

public class NMS_Server {
    // Configuration
    private static final String HOST = "10.0.7.10";
    private static final int UDP_PORT = 1234;
    private static final int TCP_PORT = 4321;

    // Sockets
    private DatagramSocket udpSocket;
    private ServerSocket tcpSocket;

    // Thread-safe Collections
    private final ConcurrentHashMap<String, AgentInfo> agents = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<String, List<MetricEntry>> metricsData = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<String, TaskInfo> tasks = new ConcurrentHashMap<>();

    // Gson for JSON processing
    private final Gson gson = new Gson();

    // Inner Classes for Data Structures
    private static class AgentInfo {
        SocketAddress address;
        long lastSeen;
    }
    @SuppressWarnings("unused")
    private static class MetricEntry {
        long timestamp;
        Map<String, Object> metrics;
    }
    @SuppressWarnings("unused")
    private static class TaskInfo {
        String taskId;
        int frequency;
        Map<String, Object> deviceMetrics;
        Map<String, Object> linkMetrics;
        Map<String, Object> alertflowConditions;
    }

    public NMS_Server() {
        try {
            udpSocket = new DatagramSocket(new InetSocketAddress(HOST, UDP_PORT));
            tcpSocket = new ServerSocket(TCP_PORT);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Load tasks from JSON configuration
    private void loadTasks() {
        try (Reader reader = new FileReader("tasks/config.json")) {
            JsonObject config = gson.fromJson(reader, JsonObject.class);

            config.getAsJsonArray("devices").forEach(deviceElement -> {
                JsonObject device = deviceElement.getAsJsonObject();
                String agentId = device.get("device_id").getAsString();

                TaskInfo task = new TaskInfo();
                task.taskId = config.get("task_id").getAsString();
                task.frequency = config.get("frequency").getAsInt();
                task.deviceMetrics = gson.fromJson(device.get("device_metrics"),
                        new TypeToken<Map<String, Object>>(){}.getType());
                task.linkMetrics = gson.fromJson(device.get("link_metrics"),
                        new TypeToken<Map<String, Object>>(){}.getType());
                task.alertflowConditions = gson.fromJson(device.get("alertflow_conditions"),
                        new TypeToken<Map<String, Object>>(){}.getType());

                tasks.put(agentId, task);
            });

            System.out.println("[SERVER] Tasks loaded.\n");
        } catch (IOException e) {
            System.err.println("[SERVER] Error loading tasks: " + e.getMessage() + "\n");
        }
    }

    // Send task to specific agents
    // Send Task using Bytes
    private void sendTaskToAgent(String agentId, SocketAddress addr) {
        TaskInfo task = tasks.get(agentId);
        if (task != null) {
            try {
                // Serialize Json
                byte[] taskMessageBytes = gson.toJson(task).getBytes(StandardCharsets.UTF_8);
                DatagramPacket packet = new DatagramPacket(
                        taskMessageBytes,
                        taskMessageBytes.length,
                        addr
                );
                udpSocket.send(packet);
                System.out.printf("[SERVER] Task sent to %s%n\n", agentId);
            } catch (IOException e) {
                System.err.printf("[SERVER] Error sending task to %s: %s%n\n", agentId, e.getMessage());
            }
        } else {
            System.out.printf("[SERVER] No task found for %s%n\n", agentId);
        }
    }


    // UDP Handshake and Communication
    private void udpListener() {
        System.out.println("[UDP] Server waiting for messages...\n");
        byte[] buffer = new byte[4096];

        while (!Thread.currentThread().isInterrupted()) {
            try {
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                udpSocket.receive(packet);

                // Obtendo a mensagem como string
                String message = new String(packet.getData(), 0, packet.getLength(), StandardCharsets.UTF_8);
                System.out.printf("[UDP] Received: %s from %s%n", message, packet.getSocketAddress());

                if (message.contains("FLAGS:SYN")) {
                    handleSynMessage(message, packet.getSocketAddress());
                } else if (message.contains("FLAGS:ACK")) {
                    handleAckMessage(message, packet.getSocketAddress());
                } else {
                    // Passar os dados como byte[] para processMetricMessage
                    byte[] messageBytes = Arrays.copyOfRange(packet.getData(), 0, packet.getLength());
                    processMetricMessage(messageBytes, packet.getSocketAddress());
                }
            } catch (IOException e) {
                System.err.println("[UDP] Error: " + e.getMessage());
            }
        }
    }


    private void handleSynMessage(String message, SocketAddress addr) throws IOException {
        Map<String, String> parts = parseMessageParts(message);
        int seq = Integer.parseInt(parts.get("SEQ"));

        String synAckMessage = String.format("SEQ:%d|ACK:%d|FLAGS:SYN-ACK", seq + 1, seq + 1);
        DatagramPacket responsePacket = new DatagramPacket(
                synAckMessage.getBytes(),
                synAckMessage.length(),
                addr
        );
        udpSocket.send(responsePacket);
        System.out.printf("[HANDSHAKE] Sent: %s%n\n", synAckMessage);
    }

    private void handleAckMessage(String message, SocketAddress addr) {
        Map<String, String> parts = parseMessageParts(message);
        String agentId = parts.getOrDefault("AGENT_ID", "Agent_" + addr);

        AgentInfo agentInfo = new AgentInfo();
        agentInfo.address = addr;
        agentInfo.lastSeen = System.currentTimeMillis();
        agents.put(agentId, agentInfo);

        System.out.printf("[HANDSHAKE] Completed for %s%n\n", agentId);
        sendTaskToAgent(agentId, addr); // Envia as tarefas para o servidor
    }

    // Método para escrever no arquivo CSV
    public static void writeToCsv(String filename, String agentId, long timestamp, int sequenceNumber, Map<String, Object> metrics) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filename, true))) {
            File file = new File(filename);
            boolean fileIsEmpty = file.length() == 0;

            if (fileIsEmpty) {
                writer.write("Agent ID, Timestamp, Sequence Number, Metric = Value\n");
            }

            // Iterar sobre as métricas e gravar no CSV, incluindo o número de sequência
            for (Map.Entry<String, Object> entry : metrics.entrySet()) {
                String metricType = entry.getKey();
                Object value = entry.getValue();
                writer.write(String.format("%s, %d, %d, %s = %s\n", agentId, timestamp, sequenceNumber, metricType, value.toString()));
            }

        } catch (IOException e) {
            System.err.printf("[CSV] Error writing to CSV: %s%n\n", e.getMessage());
        }
    }

    // Método para escrever os alertas em um arquivo CSV
    public static void writeAlertToCsv(String filename, String agentId, String alertMessage) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filename, true))) {
            File file = new File(filename);
            boolean fileIsEmpty = file.length() == 0;

            if (fileIsEmpty) {
                writer.write("Agent ID, Timestamp, Alert Message\n");
            }

            // Escrever no CSV
            writer.write(String.format("%s, \"%d\", %s\n", agentId, System.currentTimeMillis(), alertMessage));

        } catch (IOException e) {
            System.err.printf("[CSV] Error writing alert to CSV: %s%n\n", e.getMessage());
        }
    }



    private void processMetricMessage(byte[] messageBytes, SocketAddress addr) throws IOException {
        try {
            // Converter os bytes de volta para uma estrutura de dados
            String message = new String(messageBytes, StandardCharsets.UTF_8);

            // Deserializar a mensagem inteira
            Map<String, Object> metricMessage = gson.fromJson(message, new TypeToken<Map<String, Object>>() {}.getType());

            // Extrair os dados
            String agentId = (String) metricMessage.get("agent_id");
            int seqNum = ((Number) metricMessage.get("sequence_number")).intValue();
            Map<String, Object> metricDataInfo = gson.fromJson(gson.toJson(metricMessage.get("metric_data")), new TypeToken<Map<String, Object>>() {}.getType());
            long timestamp = ((Number) metricMessage.get("timestamp")).longValue();

            // Send ACK
            String ackMessage = "ACK:" + seqNum;
            byte[] ackMessageBytes = ackMessage.getBytes(StandardCharsets.UTF_8);
            DatagramPacket ackPacket = new DatagramPacket(ackMessageBytes, ackMessageBytes.length, addr);
            udpSocket.send(ackPacket);

            // Store metrics
            MetricEntry entry = new MetricEntry();
            entry.timestamp = timestamp;
            entry.metrics = metricDataInfo;

            metricsData.computeIfAbsent(agentId, k -> new ArrayList<>()).add(entry);

            System.out.printf("[METRIC] Received from %s: %s %n", agentId, metricDataInfo);

            // Write to CSV
            writeToCsv("csv/metrics_data.csv", agentId, timestamp, seqNum, metricDataInfo);

            // Update last seen
            agents.computeIfPresent(agentId, (k , v) -> {
                v.lastSeen = System.currentTimeMillis();
                return v;
            });

        } catch (Exception e) {
            System.err.printf("[METRIC] Error processing metrics: %s%n", e.getMessage());
        }
    }


    // Helper method to parse message parts
    private Map<String, String> parseMessageParts(String message) {
        Map<String, String> parts = new HashMap<>();
        for (String part : message.split("\\|")) {
            String[] kv = part.split(":");
            parts.put(kv[0], kv[1]);
        }
        return parts;
    }

    // TCP Communication
    private void handleTcpConnection(Socket conn) {
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(conn.getInputStream()))) {
            String line;
            while ((line = reader.readLine()) != null) {
                if (line.startsWith("ALERT:")) {
                    String[] parts = line.split(":", 3);
                    String agentId = parts[1].trim();
                    String alertMessage = parts[2];
                    System.out.printf("[ALERT] Received from %s: %s%n", agentId, alertMessage);

                    // Gravar o alerta no arquivo CSV
                    writeAlertToCsv("csv/alerts_data.csv", agentId, alertMessage);
                }
            }
        } catch (IOException e) {
            System.err.printf("[TCP] Error: %s%n", e.getMessage());
        }
    }

    private void tcpMainThread() {
        System.out.println("[TCP] Server waiting for connections...\n");
        while (!Thread.currentThread().isInterrupted()) {
            try {
                Socket conn = tcpSocket.accept();
                System.out.printf("[TCP] Connection received from %s%n\n", conn.getRemoteSocketAddress());
                new Thread(() -> handleTcpConnection(conn)).start();
            } catch (IOException e) {
                System.err.printf("[TCP] Error in main thread: %s%n\n", e.getMessage());
            }
        }
    }

    // Agent Activity Monitoring
    private void monitorAgents() {
        while (!Thread.currentThread().isInterrupted()) {
            try {
                // Definir o menor intervalo de verificação com base nas frequências das tarefas
                int frequency = tasks.values().stream()
                        .mapToInt(task -> task.frequency)
                        .min()
                        .orElse(20); // Valor padrão caso não existam tarefas

                Thread.sleep(5000);
                long now = System.currentTimeMillis();

                agents.entrySet().removeIf(entry ->
                        now - entry.getValue().lastSeen > (frequency) * 1000L
                );
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    private Process startIperfServer() {
        try {
            // Comando para iniciar o iperf3 no modo servidor
            String command = "iperf3 -s";

            // Criar o processo com redirecionamento para /dev/null
            ProcessBuilder processBuilder = new ProcessBuilder(command.split(" "));

            // Iniciar o processo
            Process process = processBuilder.start();

            // Log para indicar que o servidor foi iniciado
            System.out.println("\n[IPERF3] Iperf3 server started.\n");
            return process;
        } catch (IOException e) {
            System.err.println("[IPERF3] Error starting iperf3 server: " + e.getMessage() + "\n");
            return null;
        }
    }



    public void startServer() {
        // Iniciar iperf3
        Process iperfProcess = startIperfServer();

        // Registrar hook para garantir que o iperf3 seja encerrado ao finalizar o servidor
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            if (iperfProcess != null && iperfProcess.isAlive()) {
                iperfProcess.destroy();
                System.out.println("[IPERF3] Iperf3 server stopped.\n");
            }
        }));

        // Load tasks from JSON
        loadTasks();

        // Start UDP, TCP, and Agent Monitor threads
        new Thread(this::udpListener, "UDP-Listener").start();
        new Thread(this::tcpMainThread, "TCP-Main").start();
        new Thread(this::monitorAgents, "Agent-Monitor").start();
    }

    public void shutdown() {
        // Limpar o conteúdo do arquivo CSV de métricas, mas manter o cabeçalho
        try (BufferedReader reader = new BufferedReader(new FileReader("csv/metrics_data.csv"));
             BufferedWriter writer = new BufferedWriter(new FileWriter("csv/metrics_data.csv", false))) {

            String line;
            boolean headerWritten = false;

            // Lê o cabeçalho e escreve novamente
            while ((line = reader.readLine()) != null) {
                if (!headerWritten) {
                    writer.write(line + "\n");
                    headerWritten = true;
                }
            }

            System.out.println("[SERVER] CSV file for metrics cleared, header preserved.\n");
        } catch (IOException e) {
            System.err.println("[SERVER] Error clearing metrics CSV file: " + e.getMessage() + "\n");
        }

        // Limpar o conteúdo do arquivo CSV de alertas, mas manter o cabeçalho
        try (BufferedReader reader = new BufferedReader(new FileReader("csv/alerts_data.csv"));
             BufferedWriter writer = new BufferedWriter(new FileWriter("csv/alerts_data.csv", false))) {

            String line;
            boolean headerWritten = false;

            // Lê o cabeçalho e escreve novamente
            while ((line = reader.readLine()) != null) {
                if (!headerWritten) {
                    writer.write(line + "\n");
                    headerWritten = true;
                }
            }

            System.out.println("[SERVER] CSV file for alerts cleared, header preserved.\n");
        } catch (IOException e) {
            System.err.println("[SERVER] Error clearing alerts CSV file: " + e.getMessage() + "\n");
        }

        // Fechar sockets
        if (udpSocket != null) udpSocket.close();
        try {
            if (tcpSocket != null) tcpSocket.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        System.out.println("[SERVER] Sockets closed\n");
    }


    public static void main(String[] args) {
        NMS_Server server = new NMS_Server();
        Runtime.getRuntime().addShutdownHook(new Thread(server::shutdown));

        try {
            server.startServer();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
