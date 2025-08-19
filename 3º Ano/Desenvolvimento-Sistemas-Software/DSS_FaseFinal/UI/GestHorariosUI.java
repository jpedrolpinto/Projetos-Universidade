package UI;

import business.horario.SSHorarioFacade;
import business.horario.ISSHorario;
import business.uni.UC;
import business.utilizadores.Aluno;
import business.utilizadores.SSUtilizadoresFacade;
import business.utilizadores.ISSUtilizadores;
import business.horario.Horario;
import business.horario.Turno;
import business.utilizadores.Utilizador;
import data.UtilizadoresDAO;
import business.utils.utilities;

import java.util.*;

public class GestHorariosUI {
    private final ISSUtilizadores utilizadores;
    private final ISSHorario horarios;
    private final Menu menu;
    private final Menu menuAluno;
    private final Menu menuRegente;
    private final Scanner scan;
    private int currentUser;
    private String welcomeMessage;


    public GestHorariosUI() {
        this.utilizadores = new SSUtilizadoresFacade();
        this.horarios = new SSHorarioFacade();
        this.scan = new Scanner(System.in);

        this.welcomeMessage = "\n *** BEM VINDO À GESTÃO DE HORÁRIOS *** \n";

        this.menu = new Menu("Menu", new String[]{
                "Login",
                "Registar"
        });

        this.menuAluno = new Menu(new String[]{
                "Consultar horário",
                "Trocar turno",
                "Ver UCs inscritas",
                "Inscrever em UC"
        });

        this.menuRegente = new Menu(new String[]{
                "Ver turnos disponíveis",
                "Criar novo turno",
                "Inscrever todos os alunos",
                "Inscrever aluno em turno",
                "Definir preferências de turnos",
                "Ver horário geral",
                "Ver ocupação dos turnos",
                "Ver alunos inscritos"
        });

        registerHandlers();
    }

    private void registerHandlers() {
        this.menu.setHandler(1, this::login);
        this.menu.setHandler(2, this::registar);

        this.menuAluno.setHandler(1, this::consultarHorario);
        this.menuAluno.setHandler(2, this::trocarTurno);
        this.menuAluno.setHandler(3, this::verUCsInscritas);
        this.menuAluno.setHandler(4, this::inscreverEmUC);

        this.menuRegente.setHandler(1, this::verTurnosDisponiveis);
        this.menuRegente.setHandler(2, this::criarTurno);
        this.menuRegente.setHandler(3, this::alocarTodosAlunos);
        this.menuRegente.setHandler(4, this::inscreverAlunoManual);
        this.menuRegente.setHandler(5, this::definirPreferenciasTurnos);
        this.menuRegente.setHandler(6, this::verHorarioGeral);
        this.menuRegente.setHandler(7, this::verOcupacaoTurnos);
        this.menuRegente.setHandler(8, this::verAlunosInscritos);
    }

    private void updateWelcomeMessage() {
        if (currentUser > 0) {
            Utilizador user = utilizadores.getUtilizador(currentUser);
            if (user.isRegente()) {
                this.welcomeMessage = String.format("\n           *** BEM VINDO REGENTE %s *** ",
                        horarios.getRegenteName(currentUser));
            } else {
                this.welcomeMessage = String.format("\n                  *** BEM VINDO ALUNO %d *** ",
                        currentUser);
            }
        }
    }

    private void showWelcomeMessage() {
        System.out.println(welcomeMessage);
    }

    private void login() {
        int id;
        while (true) {
            try {
                System.out.print("Número de identificação: ");
                String input = scan.nextLine().trim();
                id = Integer.parseInt(input);
                break;
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        System.out.print("Password: ");
        String pass = scan.nextLine();

        if (!utilizadores.existeUtilizador(id) || !utilizadores.validaPassword(id, pass)) {
            System.out.println("Credenciais inválidas!");
            return;
        }

        currentUser = id;
        updateWelcomeMessage();
        showWelcomeMessage();

        if (utilizadores.getUtilizador(id).isRegente()) {
            menuRegente.run();
        } else {
            menuAluno.run();
        }
    }

    private void registar() {
        System.out.print("Nome: ");
        String nome = scan.nextLine();

        // Pedir e verificar email
        String email;
        boolean emailExiste;
        do {
            System.out.print("Email: ");
            email = scan.nextLine();

            // Verifica se o email já existe
            String finalEmail = email;
            emailExiste = UtilizadoresDAO.getInstance().values().stream()
                    .anyMatch(u -> u.getEmail().equalsIgnoreCase(finalEmail));

            if (emailExiste) {
                System.out.println("Este email já está registado. Por favor use outro email.");
            }
        } while (emailExiste);

        System.out.print("Password: ");
        String pass = scan.nextLine();

        System.out.print("Estatuto (1 - Normal, 2 - Especial): ");
        String estatuto = scan.nextLine().equals("1") ? "" : "Especial";

        int media;
        do {
            System.out.print("Média: ");
            try {
                media = Integer.parseInt(scan.nextLine());
                if (media >= 0 && media <= 20) {
                    break;
                }
                System.out.println("A média deve estar entre 0 e 20.");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira um número válido.");
            }
        } while (true);

        int ano;
        do {
            System.out.print("Ano (1-3): ");
            try {
                ano = Integer.parseInt(scan.nextLine());
                if (ano >= 1 && ano <= 3) {
                    break;
                }
                System.out.println("O ano deve estar entre 1 e 3.");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira um número válido.");
            }
        } while (true);

        // Buscar próximo ID disponível
        int novoId = utilities.gerarProximoId();

        try {
            utilizadores.registaUtilizador(novoId, pass, nome, email, estatuto, media, ano);
            System.out.println("Registo efetuado com sucesso! Seu número de identificação é: " + novoId);
        } catch (IllegalArgumentException e) {
            System.out.println("Erro no registo: " + e.getMessage());
        }
    }


    private void verTurnosDisponiveis() {
        List<Turno> turnos = horarios.importaTurnos();
        for (Turno t : turnos) {
            UC uc = horarios.getUC(t.getCadeira());
            System.out.printf("Turno %d: %s | Sala %s | %s | Hora %s | Capacidade %d | %s\n",
                    t.getIdTurno(),
                    uc.getNomeUC(),
                    t.getSala(),
                    t.getDataT(),
                    t.getHora(),
                    t.getCapacidade(),
                    t.isPratica() ? "Prática" : "Teórica");
        }
    }

    private void criarTurno() {
        // Mostra UCs disponíveis
        System.out.println("\nUCs disponíveis:");
        for (UC uc : horarios.getUCs()) {
            System.out.printf("%d - %s\n", uc.getIdUC(), uc.getNomeUC());
        }

        // UC ID
        int ucId;
        while (true) {
            try {
                System.out.print("\nDigite o ID da UC: ");
                ucId = Integer.parseInt(scan.nextLine().trim());
                if (horarios.getUC(ucId) != null) break;
                System.out.println("UC não encontrada. Por favor, escolha uma UC da lista.");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        // Sala
        String sala;
        do {
            System.out.print("Sala (ex: 2.18): ");
            sala = scan.nextLine().trim();
            if (sala.matches("\\d+\\.\\d+")) break;
            System.out.println("Formato inválido. Use o formato número.número (ex: 2.18)");
        } while (true);

        // Dia
        String dia;
        do {
            System.out.print("Dia (Segunda/Terça/Quarta/Quinta/Sexta): ");
            dia = scan.nextLine().trim();
            if (dia.matches("(?i)Segunda|Terça|Quarta|Quinta|Sexta")) {
                // Capitaliza primeira letra
                dia = dia.substring(0, 1).toUpperCase() + dia.substring(1).toLowerCase();
                break;
            }
            System.out.println("Por favor, insira um dia válido da semana (Segunda a Sexta).");
        } while (true);

        // Hora
        String hora;
        while (true) {
            try {
                System.out.print("Hora (formato HH/HH, ex: 9/11): ");
                hora = scan.nextLine().trim();
                String[] horas = hora.split("/");
                if (horas.length != 2) throw new NumberFormatException();

                int horaInicio = Integer.parseInt(horas[0]);
                int horaFim = Integer.parseInt(horas[1]);

                if (horaInicio >= 8 && horaFim <= 20 && horaInicio < horaFim) {
                    break;
                }
                System.out.println("O horário deve estar entre 8h e 20h, e a hora de início deve ser menor que a de fim.");
            } catch (NumberFormatException e) {
                System.out.println("Formato inválido. Use o formato HH/HH (ex: 9/11)");
            }
        }

        // Capacidade
        int capacidade;
        while (true) {
            try {
                System.out.print("Capacidade: ");
                capacidade = Integer.parseInt(scan.nextLine().trim());
                if (capacidade > 0) break;
                System.out.println("A capacidade deve ser maior que 0.");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        // Tipo de turno
        boolean pratica;
        while (true) {
            System.out.print("Tipo (1 - Prática, 2 - Teórica): ");
            String input = scan.nextLine().trim();
            if (input.equals("1") || input.equals("2")) {
                pratica = input.equals("1");
                break;
            }
            System.out.println("Por favor, escolha 1 para Prática ou 2 para Teórica.");
        }

        try {
            if (horarios.criaTurno(sala, hora, dia, capacidade, pratica, ucId)) {
                System.out.println("Turno criado com sucesso!");
            }
        } catch (IllegalArgumentException e) {
            System.out.println("Erro ao criar turno: " + e.getMessage());
        }
    }

    private void verHorarioGeral() {
        List<Turno> turnos = horarios.importaTurnos();

        System.out.println("\nHorário Geral:");
        for (Turno t : turnos) {
            UC uc = horarios.getUC(t.getCadeira());
            System.out.printf("Turno %d: %s | %s | %s | Sala %s | Capacidade %d | %s\n",
                    t.getIdTurno(),
                    uc.getNomeUC(),
                    t.getDataT(),
                    t.getHora(),
                    t.getSala(),
                    t.getCapacidade(),
                    t.isPratica() ? "Prática" : "Teórica");
        }
    }

    private void inscreverAlunoManual() {
        // Pede e valida número do aluno
        int alunoId;
        while (true) {
            try {
                System.out.print("Número do aluno: ");
                alunoId = Integer.parseInt(scan.nextLine().trim());
                if (utilizadores.existeUtilizador(alunoId)) break;
                System.out.println("Aluno não encontrado.");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        while (true) {
            // Mostra turnos disponíveis
            System.out.println("\nTurnos disponíveis:");
            for (Turno t : horarios.importaTurnos()) {
                UC uc = horarios.getUC(t.getCadeira());
                System.out.printf("Turno %d: %s | %s | %s | Sala %s | Capacidade %d | %s\n",
                        t.getIdTurno(),
                        uc.getNomeUC(),
                        t.getDataT(),
                        t.getHora(),
                        t.getSala(),
                        t.getCapacidade(),
                        t.isPratica() ? "Prática" : "Teórica");
            }

            // Pede e valida número do turno
            int turnoId;
            while (true) {
                try {
                    System.out.print("\nNúmero do turno: ");
                    turnoId = Integer.parseInt(scan.nextLine().trim());
                    if (horarios.getTurno(turnoId) != null) break;
                    System.out.println("Turno não encontrado.");
                } catch (NumberFormatException e) {
                    System.out.println("Por favor, insira apenas números.");
                }
            }

            try {
                utilizadores.alocaManual(currentUser, alunoId, turnoId);
                System.out.println("Aluno inscrito com sucesso no turno!");
                break;  // Sai do loop externo após inscrição bem-sucedida
            } catch (IllegalArgumentException e) {
                System.out.println("Erro ao inscrever aluno: " + e.getMessage());

                // Pergunta se quer tentar outro turno
                System.out.print("Deseja tentar inscrever em outro turno? (S/N): ");
                String resposta = scan.nextLine().trim().toUpperCase();

                if (!resposta.equals("S")) {
                    break;  // Sai do loop se não quiser tentar outro turno
                }
            }
        }
    }

    private void alocarTodosAlunos() {
        try {
            Map<Integer, String> resultados = utilizadores.alocaTodosAlunos(currentUser);

            System.out.println("\nResultado da alocação de todos os alunos:");
            for (Map.Entry<Integer, String> resultado : resultados.entrySet()) {
                System.out.printf("Aluno %d: %s\n", resultado.getKey(), resultado.getValue());
            }
        } catch (Exception e) {
            System.out.println("Erro ao alocar todos os alunos: " + e.getMessage());
        }
    }

    private void consultarHorario() {
        List<Horario> horariosAluno = utilizadores.consultaHorario(currentUser);
        if (horariosAluno.isEmpty()) {
            System.out.println("Sem horários registados.");
            return;
        }

        System.out.println("\nHorário do aluno:");
        for (Horario h : horariosAluno) {
            Turno t = horarios.getTurno(h.getTurno());
            UC uc = horarios.getUC(t.getCadeira());
            System.out.printf("Turno %d: %s | %s | %s | %s | %s\n",
                    t.getIdTurno(),
                    uc.getNomeUC(),
                    t.getDataT(),
                    t.getHora(),
                    t.getSala(),
                    t.isPratica() ? "Prática" : "Teórica"
            );
        }
        System.out.println();
    }

    private void verOcupacaoTurnos() {
        List<Turno> turnos = horarios.importaTurnos();
        Map<Integer, Integer> vagas = horarios.getOcupacaoTurnos();

        System.out.println("\nOcupação dos Turnos:");
        for (Turno t : turnos) {
            UC uc = horarios.getUC(t.getCadeira());
            System.out.printf("Turno %d ( %s ): Capacidade total: %d, Vagas disponíveis: %d\n",
                    t.getIdTurno(),
                    uc.getNomeUC(),
                    t.getCapacidade(),
                    vagas.get(t.getIdTurno()));
        }
    }

    private void trocarTurno() {
        List<Horario> horariosAluno = utilizadores.consultaHorario(currentUser);
        if (horariosAluno.isEmpty()) {
            System.out.println("Sem horários registados.");
            return;
        }

        // Mostra horário atual
        System.out.println("\nSeu horário atual:");
        for (Horario h : horariosAluno) {
            Turno t = horarios.getTurno(h.getTurno());
            UC uc = horarios.getUC(t.getCadeira());
            System.out.printf("Turno %d: %s | %s | %s | %s | %s\n",
                    t.getIdTurno(),
                    uc.getNomeUC(),
                    t.getDataT(),
                    t.getHora(),
                    t.getSala(),
                    t.isPratica() ? "Prática" : "Teórica"
            );
        }

        int turnoAtual;
        while (true) {
            try {
                System.out.print("\nDigite o número do turno que deseja trocar: ");
                turnoAtual = Integer.parseInt(scan.nextLine().trim());
                int finalTurnoAtual = turnoAtual;
                if (horariosAluno.stream().anyMatch(h -> h.getTurno() == finalTurnoAtual)) break;
                System.out.println("Turno não encontrado no seu horário!");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        List<Turno> turnosCompativeis = utilizadores.getTurnosCompativeis(currentUser, turnoAtual);

        if (turnosCompativeis.isEmpty()) {
            System.out.println("Não há turnos compatíveis disponíveis para troca!");
            return;
        }

        // Mostra turnos compatíveis
        System.out.println("\nTurnos compatíveis disponíveis:");
        for (Turno t : turnosCompativeis) {
            UC uc = horarios.getUC(t.getCadeira());
            System.out.printf("Turno %d: %s | %s | %s | %s | %s\n",
                    t.getIdTurno(),
                    uc.getNomeUC(),
                    t.getDataT(),
                    t.getHora(),
                    t.getSala(),
                    t.isPratica() ? "Prática" : "Teórica"
            );
        }

        int novoTurno;
        while (true) {
            try {
                System.out.print("\nDigite o número do turno para qual deseja trocar: ");
                novoTurno = Integer.parseInt(scan.nextLine().trim());
                int finalNovoTurno = novoTurno;
                if (turnosCompativeis.stream().anyMatch(t -> t.getIdTurno() == finalNovoTurno)) break;
                System.out.println("Por favor, escolha um dos turnos compatíveis listados.");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        try {
            utilizadores.trocarTurno(currentUser, turnoAtual, novoTurno);
            System.out.println("Troca realizada com sucesso!");
        } catch (IllegalArgumentException e) {
            System.out.println("Erro ao realizar a troca: " + e.getMessage());
        }
    }

    private void verAlunosInscritos() {
        List<Aluno> alunos = horarios.importaAlunos();
        System.out.println("\nAlunos inscritos no curso:");
        for (Aluno a : alunos) {
            System.out.printf("ID: %d | Nome: %s | Média: %d | Estatuto: %s\n",
                    a.getID(),
                    a.getNome(),
                    a.getMedia_Curso(), // Usando %d para inteiro em vez de %.2f
                    a.getEstatuto().isEmpty() ? "Normal" : a.getEstatuto());
        }
    }

    private void definirPreferenciasTurnos() {
        verTurnosDisponiveis();

        int idTurno;
        while (true) {
            try {
                System.out.print("\nDigite o número do turno para definir preferência: ");
                idTurno = Integer.parseInt(scan.nextLine().trim());
                break;
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        System.out.println("\nTipos de preferência disponíveis:");
        System.out.println("1 - Estatuto Especial");
        System.out.println("2 - Média Mínima");

        int opcao;
        while (true) {
            try {
                System.out.print("Escolha o tipo de preferência: ");
                opcao = Integer.parseInt(scan.nextLine().trim());
                if (opcao == 1 || opcao == 2) break;
                System.out.println("Por favor, escolha 1 ou 2.");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        String tipoPreferencia;
        String valorPreferencia;

        if (opcao == 1) {
            tipoPreferencia = "ESTATUTO_ESPECIAL";
            do {
                System.out.print("Digite o estatuto (ex: Especial): ");
                valorPreferencia = scan.nextLine().trim();
                if (!valorPreferencia.isEmpty()) break;
                System.out.println("O estatuto não pode estar vazio.");
            } while (true);
        } else {
            tipoPreferencia = "MEDIA_MINIMA";
            while (true) {
                try {
                    System.out.print("Digite a média mínima: ");
                    int media = Integer.parseInt(scan.nextLine().trim());
                    if (media >= 0 && media <= 20) {
                        valorPreferencia = String.valueOf(media);
                        break;
                    }
                    System.out.println("A média deve estar entre 0 e 20.");
                } catch (NumberFormatException e) {
                    System.out.println("Por favor, insira apenas números.");
                }
            }
        }

        try {
            utilizadores.definirPreferenciaTurno(currentUser, idTurno, tipoPreferencia, valorPreferencia);
            System.out.println("Preferência definida com sucesso!");
        } catch (IllegalArgumentException e) {
            System.out.println("Erro ao definir preferência: " + e.getMessage());
        }
    }

    private void verUCsInscritas() {
        List<UC> ucs = utilizadores.consultarUCsInscritas(currentUser);
        if (ucs.isEmpty()) {
            System.out.println("Não está inscrito em nenhuma UC.");
            return;
        }

        System.out.println("\nUCs em que está inscrito:");
        for (UC uc : ucs) {
            System.out.printf("%d - %s (Ano: %d, Semestre: %d)\n",
                    uc.getIdUC(),
                    uc.getNomeUC(),
                    uc.getAno(),
                    uc.getSemestre());
        }
    }

    private void inscreverEmUC() {
        // Mostra UCs disponíveis
        System.out.println("\nUCs disponíveis:");
        for (UC uc : horarios.getUCs()) {
            System.out.printf("%d - %s (Ano: %d, Semestre: %d)\n",
                    uc.getIdUC(),
                    uc.getNomeUC(),
                    uc.getAno(),
                    uc.getSemestre());
        }

        // Pede e valida número da UC
        int ucId;
        while (true) {
            try {
                System.out.print("\nDigite o ID da UC em que deseja se inscrever: ");
                ucId = Integer.parseInt(scan.nextLine().trim());
                if (horarios.getUC(ucId) != null) break;
                System.out.println("UC não encontrada. Por favor, escolha uma UC da lista.");
            } catch (NumberFormatException e) {
                System.out.println("Por favor, insira apenas números.");
            }
        }

        try {
            utilizadores.inscreverUC(currentUser, ucId);
            System.out.println("Inscrito com sucesso na UC!");
        } catch (IllegalArgumentException e) {
            System.out.println("Erro ao inscrever na UC: " + e.getMessage());
        }
    }

    public void run() {
        this.menu.run();
        System.out.println("Até breve!...");
    }

}
