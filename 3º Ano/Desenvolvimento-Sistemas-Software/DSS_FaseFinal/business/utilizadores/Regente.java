package business.utilizadores;

import business.horario.Horario;
import business.horario.Turno;
import business.horario.TurnoPreferencia;
import business.utils.utilities;
import data.*;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.stream.Collectors;


public class Regente extends Utilizador {
    private int id_Regente;
    private String Nome_Regente;
    private final Map<Integer, Utilizador> utilizadores;
    private final Map<Integer, List<Horario>> horarios;
    private final Map<Integer, Turno> turnos;
    private final Map<Integer, TurnoPreferencia> preferencias;

    public Regente(String Nome_Regente){
        this.id_Regente = 0;
        this.Nome_Regente = Nome_Regente;
        this.utilizadores = UtilizadoresDAO.getInstance();
        this.horarios = HorarioDAOWraper.getInstance();
        this.turnos = TurnoDAO.getInstance();
        this.preferencias = new HashMap<>();
    }

    public Regente(int id_Regente, String Nome_Regente){
        this.id_Regente = id_Regente;
        this.Nome_Regente = Nome_Regente;
        this.utilizadores = UtilizadoresDAO.getInstance();
        this.horarios = HorarioDAOWraper.getInstance();
        this.turnos = TurnoDAO.getInstance();
        this.preferencias = new HashMap<>();
    }

    private Regente(Map<Integer, Utilizador> utilizadores, Map<Integer, List<Horario>> horarios, Map<Integer, Turno> turnos, Map<Integer, TurnoPreferencia> preferencias){
        this.utilizadores = utilizadores;
        this.horarios = horarios;
        this.turnos = turnos;
        this.preferencias = preferencias;
    }

    public int getId_Regente(){
        return id_Regente;
    }

    public void setId_Regente(int id_Regente){
        this.id_Regente = id_Regente;
    }

    public String getNome_Regente(){
        return Nome_Regente;
    }

    public void setNome_Regente(String Nome_Regente){
        this.Nome_Regente = Nome_Regente;
    }

    private boolean verificaPreferencias(Turno turno, Aluno aluno) {
        TurnoPreferencia pref = TurnoPreferenciaDAO.getInstance().get(turno.getIdTurno());

        if (pref == null) return true;

        switch (pref.getTipoPreferencia()) {
            case "ESTATUTO_ESPECIAL":
                return aluno.getEstatuto().equals(pref.getValorPreferencia());
            case "MEDIA_MINIMA":
                return aluno.getMedia_Curso() >= Double.parseDouble(pref.getValorPreferencia());
            default:
                return true;
        }
    }

    public void definirPreferenciaTurno(int idTurno, String tipoPreferencia, String valorPreferencia) {
        if (!turnos.containsKey(idTurno)) {
            throw new IllegalArgumentException("Turno não encontrado");
        }

        // Criar a preferência com o ID do regente
        TurnoPreferencia pref = new TurnoPreferencia(
                this.id_Regente,  // Adicionar o ID do regente
                idTurno,
                tipoPreferencia,
                valorPreferencia
        );

        // Persistir no DAO
        TurnoPreferenciaDAO.getInstance().put(idTurno, pref);

        // Atualizar o mapa local de preferências
        preferencias.put(idTurno, pref);
    }


    public List<Horario> alocaAluno(int Numero_id) {
        if (!utilizadores.containsKey(Numero_id)) {
            throw new IllegalArgumentException("Utilizador não encontrado");
        }
        Aluno aluno = AlunoDAO.getInstance().get(Numero_id);

        // Obter as UCs em que o aluno está inscrito
        List<Integer> ucsInscritas = aluno.getUCsInscritas().stream()
                .map(uc -> uc.getIdUC())
                .collect(Collectors.toList());

        // Filtrar turnos apenas das UCs inscritas e com capacidade disponível
        List<Turno> turnosDisponiveis = turnos.values().stream()
                .filter(t -> t.getCapacidade() > 0) // Verifica capacidade antes de tentar alocar
                .filter(t -> ucsInscritas.contains(t.getCadeira()))
                .filter(t -> verificaPreferencias(t, aluno))
                .sorted(Comparator.comparing(Turno::getIdTurno))
                .collect(Collectors.toList());

        if (turnosDisponiveis.isEmpty()) {
            throw new IllegalStateException("Não existem turnos disponíveis para as UCs inscritas");
        }

        List<Horario> horariosAluno = new ArrayList<>();
        Map<Integer, Set<String>> ucAlocadas = new HashMap<>();

        // Tenta alocar o aluno nos turnos disponíveis
        for (Turno turno : turnosDisponiveis) {
            try {
                // Verifica se ainda tem capacidade (double-check por causa da concorrência)
                if (turno.getCapacidade() <= 0) {
                    continue;
                }

                int ucId = turno.getCadeira();
                String tipoAula = turno.isPratica() ? "P" : (turno.isTeorica() ? "T" : "O");
                Set<String> tiposAlocados = ucAlocadas.getOrDefault(ucId, new HashSet<>());

                if (tiposAlocados.isEmpty() || !tiposAlocados.contains(tipoAula)) {
                    boolean hasConflict = horariosAluno.stream()
                            .map(h -> turnos.get(h.getTurno()))
                            .anyMatch(t -> utilities.hasTimeConflict(t, turno));

                    if (!hasConflict) {
                        Horario newHorario = new Horario(
                                Numero_id,
                                1,
                                turno.getIdTurno()
                        );

                        // Decrementa a capacidade e atualiza o turno
                        turno.setCapacidade(turno.getCapacidade() - 1);
                        turnos.put(turno.getIdTurno(), turno);

                        horariosAluno.add(newHorario);
                        tiposAlocados.add(tipoAula);
                        ucAlocadas.put(ucId, tiposAlocados);
                    }
                }
            } catch (Exception e) {
                System.err.println("Erro ao alocar turno " + turno.getIdTurno() + ": " + e.getMessage());
            }
        }

        if (!horariosAluno.isEmpty()) {
            horarios.put(Numero_id, horariosAluno);
        } else {
            throw new IllegalStateException("Não foi possível alocar nenhum turno");
        }

        return horariosAluno;
    }

    public Map<Integer, String> alocaTodosAlunos() {
        List<Integer> alunos = utilizadores.values().stream()
                .filter(u -> !u.isRegente())
                .map(Utilizador::getNumeroID)
                .sorted()
                .collect(Collectors.toList());

        // Restaura as capacidades usando o mapa de ocupação
        Map<Integer, Integer> ocupacaoAtual = new Turno().getOcupacaoTurnos(turnos, horarios);
        for (Turno turno : turnos.values()) {
            int ocupacao = ocupacaoAtual.getOrDefault(turno.getIdTurno(), 0);
            int capacidadeOriginal = turno.getCapacidade() + ocupacao;
            turno.setCapacidade(capacidadeOriginal);
        }

        // Limpa todas as alocações existentes
        horarios.clear();

        Map<Integer, String> resultados = new ConcurrentHashMap<>();
        AtomicBoolean sucesso = new AtomicBoolean(true);

        try {
            int batchSize = 10;
            for (int i = 0; i < alunos.size(); i += batchSize) {
                int end = Math.min(i + batchSize, alunos.size());
                List<Integer> batch = alunos.subList(i, end);

                batch.parallelStream().forEach(alunoId -> {
                    try {
                        alocaAluno(alunoId);
                        resultados.put(alunoId, "Alocação completa com sucesso");
                    } catch (Exception e) {
                        sucesso.set(false);
                        resultados.put(alunoId, "Erro na alocação: " + e.getMessage());
                    }
                });

                if (!sucesso.get()) {
                    break;
                }
            }

            return resultados;

        } catch (Exception e) {
            throw new IllegalStateException("Erro na alocação geral: " + e.getMessage());
        }
    }

    public void alocaManual(int numeroAluno, int turnoId) {
        if (!utilizadores.containsKey(numeroAluno)) {
            throw new IllegalArgumentException("Aluno não encontrado");
        }

        Turno turno = turnos.get(turnoId);
        if (turno == null) {
            throw new IllegalArgumentException("Turno não encontrado");
        }

        if (turno.getCapacidade() <= 0) {
            throw new IllegalArgumentException("Turno sem vagas disponíveis");
        }

        Aluno aluno = AlunoDAO.getInstance().get(numeroAluno);

        // Verifica se aluno está inscrito na UC do turno
        if (!aluno.getUCsInscritas().stream().anyMatch(uc -> uc.getIdUC() == turno.getCadeira())) {
            throw new IllegalArgumentException("Aluno não está inscrito na UC deste turno");
        }

        TurnoPreferencia pref = preferencias.get(turnoId);
        if (pref != null && !verificaPreferencias(turno, aluno)) {
            throw new IllegalArgumentException("Aluno não atende aos requisitos preferenciais do turno");
        }

        List<Horario> horariosAluno = horarios.getOrDefault(numeroAluno, new ArrayList<>());
        boolean temConflito = horariosAluno.stream()
                .map(h -> turnos.get(h.getTurno()))
                .anyMatch(t -> t.getCadeira() == turno.getCadeira() &&
                        t.isPratica() == turno.isPratica());

        if (temConflito) {
            throw new IllegalArgumentException("Aluno já tem um turno deste tipo para esta UC");
        }

        boolean temConflitosHorario = horariosAluno.stream()
                .map(h -> turnos.get(h.getTurno()))
                .anyMatch(t -> utilities.hasTimeConflict(t, turno));

        if (temConflitosHorario) {
            throw new IllegalArgumentException("Existe conflito de horário com outro turno");
        }

        Horario novoHorario = new Horario(numeroAluno, 1, turnoId);
        turno.setCapacidade(turno.getCapacidade() - 1);
        turnos.put(turnoId, turno);
        horariosAluno.add(novoHorario);
        horarios.put(numeroAluno, horariosAluno);
    }
}
