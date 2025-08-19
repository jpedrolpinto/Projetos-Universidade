import java.io.*;
import java.net.*;
import java.util.*;
import java.time.Instant;
import java.lang.management.*;
import java.util.concurrent.*;
import com.sun.management.OperatingSystemMXBean;

import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;

public class NMS_Agent {
    // CONFIG
    private static final String HOST = "10.0.7.10";
    private static final int UDP_PORT = 1234;
    private static final int TCP_PORT = 4321;

    // Agent Identification
    private static final String AGENT_ID = getHostName();

    // Global Variables
    private static JSONObject task = new JSONObject();
    private static JSONObject alertConditions = new JSONObject();
    private static int sequenceNumber = 0;
    private static final Object udpLock = new Object();

    // Tabela para armazenar tarefas recebidas (Concorrente para segurança em threads)
    private static final Map<Integer, JSONObject> taskRegistry = new ConcurrentHashMap<>();
    private static int taskCounter = 0;

    // Sockets
    private static DatagramSocket udpSocket;
    private static Socket tcpSocket;

    // Helper method to get hostname
    public static String getHostName() {
        try {
            ProcessBuilder processBuilder = new ProcessBuilder("hostname");
            Process process = processBuilder.start();
            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            return reader.readLine();  // Lê o nome do host
        } catch (IOException e) {
            e.printStackTrace();
            return "UnknownAgent";
        }
    }

    // UDP Handshake Method
    private static boolean udpHandshake() {
        int initialSeq = 1;
        int maxRetransmissions = 5;
        int timeout = 2000; // 2 seconds

        for (int retransmissions = 0; retransmissions < maxRetransmissions; retransmissions++) {
            try {
                // Create SYN message
                String synMessage = String.format("SEQ:%d|ACK:0|FLAGS:SYN|AGENT_ID:%s",
                        initialSeq, AGENT_ID);

                // Send SYN
                byte[] sendData = synMessage.getBytes();
                DatagramPacket sendPacket = new DatagramPacket(sendData,
                        sendData.length,
                        InetAddress.getByName(HOST),
                        UDP_PORT);
                udpSocket.send(sendPacket);
                System.out.printf("[HANDSHAKE] Sent SYN (tentativa %d): %s%n\n",
                        retransmissions + 1, synMessage);

                // Receive SYN-ACK
                byte[] receiveData = new byte[1024];
                DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
                udpSocket.setSoTimeout(timeout);
                udpSocket.receive(receivePacket);

                String message = new String(receivePacket.getData(), 0, receivePacket.getLength());
                System.out.printf("[HANDSHAKE] Received: %s%n\n", message);

                // Validate SYN-ACK
                if (message.contains("FLAGS:SYN-ACK")) {
                    Map<String, String> parts = parseMessageParts(message);
                    int ackNumber = Integer.parseInt(parts.get("ACK"));

                    if (ackNumber == initialSeq + 1) {
                        int serverSeq = Integer.parseInt(parts.get("SEQ"));

                        // Send final ACK
                        String ackMessage = String.format("SEQ:%d|ACK:%d|FLAGS:ACK|AGENT_ID:%s",
                                serverSeq + 1, serverSeq + 1, AGENT_ID);
                        byte[] ackData = ackMessage.getBytes();
                        DatagramPacket ackPacket = new DatagramPacket(ackData,
                                ackData.length,
                                InetAddress.getByName(HOST),
                                UDP_PORT);
                        udpSocket.send(ackPacket);
                        System.out.printf("[HANDSHAKE] Sent ACK: %s%n\n", ackMessage);

                        return true;
                    }
                }
            } catch (SocketTimeoutException e) {
                System.out.println("[HANDSHAKE] Timeout, retransmitindo...\n");
            } catch (Exception e) {
                System.out.printf("[HANDSHAKE] Error: %s%n\n", e.getMessage());
            }
        }

        System.out.println("[HANDSHAKE] Handshake falhou após máximo de retransmissões.\n");
        return false;
    }

    // Parse message parts
    private static Map<String, String> parseMessageParts(String message) {
        Map<String, String> parts = new HashMap<>();
        for (String part : message.split("\\|")) {
            String[] kv = part.split(":");
            parts.put(kv[0], kv[1]);
        }
        return parts;
    }

    // Método para registrar a tarefa recebida
    private static void registerTask(JSONObject receivedTask) {
        synchronized (taskRegistry) {
            taskCounter++;
            taskRegistry.put(taskCounter, receivedTask);
            System.out.printf("[TASK_REGISTRY] Task ID %d registered: %s%n\n", taskCounter, receivedTask.toJSONString());
        }
    }

    // Método para listar todas as tarefas registradas
    private static void listRegisteredTasks() {
        synchronized (taskRegistry) {
            System.out.println("[TASK_REGISTRY] Tasks registradas:");
            for (Map.Entry<Integer, JSONObject> entry : taskRegistry.entrySet()) {
                System.out.printf("ID: %d | Tarefa: %s%n\n", entry.getKey(), entry.getValue().toJSONString());
            }
        }
    }

    // Receive Task from Server
    private static void receiveTask() {
        try {
            udpSocket.setSoTimeout(20000); // 20 seconds timeout
            byte[] receiveData = new byte[4096];
            DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
            udpSocket.receive(receivePacket);

            String message = new String(receivePacket.getData(), 0, receivePacket.getLength());
            System.out.printf("[TASK] Received: %s%n\n", message);

            JSONParser parser = new JSONParser();
            task = (JSONObject) parser.parse(message);

            // Processar alertflowConditions
            Object alertConditionsObj = task.get("alertflowConditions");
            if (alertConditionsObj instanceof JSONObject) {
                alertConditions = (JSONObject) alertConditionsObj;
            } else {
                alertConditions = new JSONObject();
            }

            // Registra a tarefa recebida
            registerTask(task);

        } catch (Exception e) {
            System.out.printf("[TASK] Erro ao receber tarefa: %s%n\n", e.getMessage());
            task = new JSONObject();
            alertConditions = new JSONObject();
        }
    }

    // Atualizações no metodo collectMetrics
    private static Map<String, Object> collectMetrics() {
        Map<String, Object> metricData = new HashMap<>();

        // Verificar e processar métricas do dispositivo
        if (task.containsKey("deviceMetrics")) {
            JSONObject deviceMetrics = (JSONObject) task.get("deviceMetrics");

            // Uso de CPU
            if (Boolean.TRUE.equals(deviceMetrics.get("cpu_usage"))) {
                double cpuUsage = getCpuUsage();
                metricData.put("cpu_usage", String.format("%.2f",cpuUsage));

                Double cpuThreshold = (Double) alertConditions.get("cpu_usage");
                if (cpuThreshold != null && cpuUsage > cpuThreshold) {
                    sendAlert(String.format("ALERTA: CPU_USAGE excedido: %.2f%%", cpuUsage));
                }
            }

            // Uso de RAM
            if (Boolean.TRUE.equals(deviceMetrics.get("ram_usage"))) {
                double ramUsage = getMemoryUsage();
                metricData.put("ram_usage", String.format("%.2f",ramUsage));

                Double ramThreshold = (Double) alertConditions.get("ram_usage");
                if (ramThreshold != null && ramUsage > ramThreshold) {
                    sendAlert(String.format("ALERTA: RAM_USAGE excedido: %.2f%%", ramUsage));
                }
            }
        } else {
            System.out.println("[ERROR] deviceMetrics não está presente ou é inválido.");
        }

        // Verificar e processar métricas de link
        if (task.containsKey("linkMetrics")) {
            JSONObject linkMetrics = (JSONObject) task.get("linkMetrics");

            // Latência
            if (linkMetrics.containsKey("latency")) {
                JSONObject latencyObject = (JSONObject) linkMetrics.get("latency");

                // Garantir que os valores passados são válidos
                String pingDestination = (String) latencyObject.get("ping_destination");
                int count = ((Number) latencyObject.get("count")).intValue();  // Garantir que 'count' é um inteiro
                int frequency = ((Number) latencyObject.get("frequency")).intValue();  // Garantir que 'frequency' é um inteiro

                double latency = getPing(pingDestination, count, frequency);
                metricData.put("latency", String.format("%.2f",latency));

                Double latencyThreshold = (Double) alertConditions.get("latency");
                if (latencyThreshold != null && latency > latencyThreshold) {
                    sendAlert(String.format("ALERTA: LATENCY excedido: %.2f ms", latency));
                }
            }
            // Perda de pacotes
            if (linkMetrics.containsKey("packet_loss")) {
                JSONObject packetLossObject = (JSONObject) linkMetrics.get("packet_loss");

                boolean enabled = Boolean.TRUE.equals(packetLossObject.get("enabled"));
                String serverIp = (String) packetLossObject.get("server_ip");
                int frequency = ((Number) packetLossObject.get("frequency")).intValue();

                if (enabled && serverIp != null && !serverIp.isEmpty() && frequency > 0) {
                    double packetLoss = getIperf(serverIp, frequency);
                    metricData.put("packet_loss", String.format("%.2f",packetLoss));

                    Double packetLossThreshold = (Double) alertConditions.get("packet_loss");
                    if (packetLossThreshold != null && packetLoss > packetLossThreshold) {
                        sendAlert(String.format("ALERTA: PACKET_LOSS excedido: %.2f%%", packetLoss));
                    }
                } else {
                    System.out.println("[ERROR] Configurações de packet_loss inválidas ou desabilitadas.");
                }
            }
        } else {
            System.out.println("[ERROR] linkMetrics não está presente ou é inválido.");
        }
        return metricData;
    }

    // Get CPU Usage
    public static double getCpuUsage() {
        OperatingSystemMXBean osBean = (OperatingSystemMXBean) ManagementFactory.getOperatingSystemMXBean();

        // Obtém o tempo de uso da CPU e tempo total do sistema.
        double systemCpuLoad = osBean.getSystemCpuLoad();
        
        if (systemCpuLoad < 0) {
            // -1 significa que o cálculo não pode ser feito (geralmente em sistemas virtuais ou ambientes limitados).
            return 0.0;
        }
        
        return systemCpuLoad * 100.0; // Retorna a percentagem de uso da CPU.
    }


    // Get Memory Usage
    private static double getMemoryUsage() {
        Runtime runtime = Runtime.getRuntime();
        long usedMemory = runtime.totalMemory() - runtime.freeMemory();
        long maxMemory = runtime.maxMemory();
        return (double) usedMemory / maxMemory * 100.0;
    }

    // Get Ping
    private static double getPing(String host, int count, int frequency) {
        try {
            InetAddress address = InetAddress.getByName(host);
            long totalPingTime = 0;
            int successfulPings = 0;

            for (int i = 0; i < count; i++) {
                long start = System.currentTimeMillis();

                if (address.isReachable(1000)) {
                    long end = System.currentTimeMillis();
                    totalPingTime += (end - start);
                    successfulPings++;
                } else {
                    System.out.printf("Ping %d falhou%n", i+1);
                }

                // Espera pela frequência especificada (em milissegundos)
                Thread.sleep(frequency);
            }

            if (successfulPings > 0) {
                return Math.max(1, (double) totalPingTime / successfulPings);
            } else {
                return -1; // Caso nenhum ping tenha sido bem-sucedido
            }

        } catch (IOException | InterruptedException e) {
            System.out.printf("Erro ao obter ping: %s%n", e.getMessage());
        }
        return -1;
    }

    private static double getIperf(String serverIp, int frequency) {
        try {
            // Verifica se o comando 'iperf3' está disponível no sistema
            ProcessBuilder checkIperfBuilder = new ProcessBuilder("which", "iperf3");
            Process checkIperf = checkIperfBuilder.start();

            if (checkIperf.waitFor() != 0) {
                System.out.println("[IPERF] O comando 'iperf3' não está instalado ou acessível.\n");
                return -1;
            }

            // Configura o comando iperf3 para execução
            List<String> command = Arrays.asList("iperf3", "-c", serverIp, "-u", "-t", String.valueOf(frequency));
            ProcessBuilder processBuilder = new ProcessBuilder(command);
            Process process = processBuilder.start();

            // Lê a saída do processo de forma eficiente usando um Scanner
            try (Scanner scanner = new Scanner(process.getInputStream())) {
                double packetLoss = -1;

                // Procura pela linha que contém a métrica de perda de pacotes
                while (scanner.hasNextLine()) {
                    String line = scanner.nextLine();
                    if(line.contains("Connecting to host") || line.contains("port")){
                        continue;
                    }
                    if (line.contains("receiver") && line.contains("sec")) {
                        String[] parts = line.split(" ");
                        for (String part : parts) {

                            if (part.contains("%")) {
                                try {
                                    // Extrai e converte o valor de perda de pacotes
                                    String lossValue = part.replaceAll("[^0-9.]", "").trim();
                                    packetLoss = Double.parseDouble(lossValue);
                                    if(packetLoss > 100){return -1;}
                                    return packetLoss;
                                } catch (NumberFormatException e) {
                                    System.out.printf("[IPERF] Erro ao converter perda de pacotes: %s%n", e.getMessage());
                                    return -1;
                                }
                            }
                        }
                    }
                }

                // Se não encontrar a métrica de perda de pacotes, retorna -1
                return -1;
            } finally {
                process.waitFor();
            }
        } catch (IOException | InterruptedException e) {
            System.out.printf("[IPERF] Erro ao executar o iperf3: %s%n\n", e.getMessage());
            return -1;
        }
    }


    // Send Metrics
    @SuppressWarnings("unchecked")
    private static void sendMetrics() {
        int maxRetransmissions = 5;
        int timeout = 5000; // 5 seconds


        while (true) {
            if (!task.isEmpty()) {
                // Coleta as metricas, aqui envia os alertas caso passe
                Map<String, Object> metricData = collectMetrics();
                long frequency = (Long) task.get("frequency");

                // Prepare metric message using HashMap<String, Object> para evitar o erro de tipo
                //prepara a mensagem com o numero de sequencia, o id e o tempo
                Map<String, Object> metricMessage = new HashMap<>();
                metricMessage.put("sequence_number", sequenceNumber);
                metricMessage.put("agent_id", AGENT_ID);
                metricMessage.put("timestamp", Instant.now().getEpochSecond());

                // Converter metricData para JSONObject
                //consistencia do tipo de mensagem, suporta estruturas mais complexas
                JSONObject metricDataJson = new JSONObject();
                for (Map.Entry<String, Object> entry : metricData.entrySet()) {
                    metricDataJson.put(entry.getKey(), entry.getValue()); // Adiciona os dados ao JSONObject
                }
                metricMessage.put("metric_data", metricDataJson);

                // Agora convertendo metricMessage em JSON
                //desnecessario
                JSONObject finalMessage = new JSONObject();
                for (Map.Entry<String, Object> entry : metricMessage.entrySet()) {
                    finalMessage.put(entry.getKey(), entry.getValue()); // Adiciona os dados ao JSONObject final
                }

                String message = finalMessage.toString(); // Converte a HashMap para String JSON
                int attempts = 0;

                while (attempts < maxRetransmissions && !Thread.currentThread().isInterrupted()) {
                    try {
                        synchronized (udpLock) {
                            byte[] sendData = message.getBytes();
                            DatagramPacket sendPacket = new DatagramPacket(sendData,
                                    sendData.length,
                                    InetAddress.getByName(HOST),
                                    UDP_PORT);
                            udpSocket.send(sendPacket);
                            System.out.printf("[METRIC] Sent: %s (tentativa %d)%n",
                                    metricData, attempts + 1);
                        }

                        // Wait for ACK
                        byte[] receiveData = new byte[1024];
                        DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
                        udpSocket.setSoTimeout(timeout);
                        udpSocket.receive(receivePacket);

                        String ackMsg = new String(receivePacket.getData(), 0, receivePacket.getLength());
                        if (ackMsg.equals("ACK:" + sequenceNumber)) {
                            System.out.println("[METRIC] ACK recebido do servidor");
                            sequenceNumber++;
                            break;
                        }

                        attempts++;
                    } catch (SocketTimeoutException e) {
                        System.out.println("[METRIC] Timeout esperando ACK, retransmitindo...\n");
                    } catch (IOException e) {
                        System.err.println("[METRIC] Erro de comunicação com o servidor: " + e.getMessage());
                        break; // Interrompe o loop em caso de erro de I/O
                    }
                }

                if (attempts >= maxRetransmissions) {
                    System.out.println("[METRIC] Falha no envio de métricas após máximo de retransmissões.\n");
                }

                try {
                    Thread.sleep(frequency * 1000);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            } else {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }



    // Send Alert via TCP
    private static void sendAlert(String alertMessage) {
        try {
            PrintWriter out = new PrintWriter(tcpSocket.getOutputStream(), true);
            out.println("ALERT:" + AGENT_ID + ":" + alertMessage);
            System.out.printf("[ALERT] Sent: %s%n", alertMessage);
        } catch (IOException e) {
            System.out.printf("[ALERT] Erro ao enviar alerta: %s%n\n", e.getMessage());
        }
    }

    // TCP Connect
    private static void tcpConnect() {
        try {
            tcpSocket = new Socket(HOST, TCP_PORT);
            System.out.println("[TCP] Conectado ao servidor para envio de alertas.\n");
        } catch (IOException e) {
            System.out.printf("[TCP] Erro ao conectar: %s%n\n", e.getMessage());
        }
    }

    // Adição de uma interface simples para comandos do usuário
    private static void commandInterface() {
        try(Scanner scanner = new Scanner(System.in)){
            while (true) {
                System.out.print("[AGENT] Comando ('list_tasks', 'exit'):\n");
                String command = scanner.nextLine();
                if ("list_tasks".equalsIgnoreCase(command)) {
                    listRegisteredTasks();
                } else if ("exit".equalsIgnoreCase(command)) {
                    System.out.println("[AGENT] Encerrando interface de comandos...\n");
                    break;
                } else {
                    System.out.println("[AGENT] Comando desconhecido!\n");
                }
            }
        } catch(Exception e ){
            e.printStackTrace();
        }
    }


    // Main Agent Method
    private static void startAgent() {
        try {
            // Initialize UDP Socket
            udpSocket = new DatagramSocket();
            System.out.printf("[AGENT] ID: %s%n\n", AGENT_ID);

            // Perform UDP Handshake
            if (!udpHandshake()) {
                throw new IOException("[HANDSHAKE] Falha na conexão com o servidor!\n");
            }

            System.out.println("[HANDSHAKE] Conexão estabelecida com o servidor!\n");

            // Receive Task from Server
            receiveTask();

            // Connect via TCP for Alerts
            tcpConnect();

            // Start Metrics Thread
            Thread metricsThread = new Thread(NMS_Agent::sendMetrics);
            metricsThread.setDaemon(true);
            metricsThread.start();

            // Thread para interface de comandos
            Thread commandThread = new Thread(NMS_Agent::commandInterface);
            commandThread.setDaemon(true);
            commandThread.start();

            // Loop principal do agente
            while (true) {
                try {
                    Thread.sleep(1000); // Mantém o agente ativo
                } catch (InterruptedException e) {
                    System.out.println("[AGENT] Agente interrompido durante o sleep.\n");
                    break;
                }
            }
        } catch (Exception e) {
            System.out.printf("[AGENT] Erro: %s%n\n", e.getMessage());
        } finally {
            // Close sockets
            if (udpSocket != null) udpSocket.close();
            try {
                if (tcpSocket != null) tcpSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void main(String[] args) {
        startAgent();
    }
}

