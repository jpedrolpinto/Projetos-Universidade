package business.utilizadores;
import business.horario.Horario;
import business.horario.Turno;
import business.horario.TurnoPreferencia;
import business.uni.Inscricao;
import business.uni.UC;
import business.utils.utilities;
import data.*;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

public class Aluno extends Utilizador {
    private int id;
    private String Nome;
    private String Estatuto;
    private int Ano_Inscricao;
    private int Media_Curso;
    private List<Horario> horarios;
    private final Map<Integer, Utilizador> utilizadores;
    private final Map<Integer, List<Horario>> horariosMap;
    private final Map<Integer, Turno> turnos;
    private final Map<Integer, TurnoPreferencia> preferencias;

    public Aluno(int id){
        super();
        this.id = id;
        this.Nome = "";
        this.Estatuto = "";
        this.Ano_Inscricao = 0;
        this.Media_Curso = 0;
        this.utilizadores = UtilizadoresDAO.getInstance();
        this.horariosMap = HorarioDAOWraper.getInstance();
        this.turnos = TurnoDAO.getInstance();
        this.preferencias = new HashMap<>();
    }

    public Aluno(int id, String Nome, String Estatuto, int Ano_inscricao, int Media_Curso){
        this.id = id;
        this.Nome = Nome;
        this.Estatuto = Estatuto;
        this.Ano_Inscricao = Ano_inscricao;
        this.Media_Curso = Media_Curso;
        this.utilizadores = UtilizadoresDAO.getInstance();
        this.horariosMap = HorarioDAOWraper.getInstance();
        this.turnos = TurnoDAO.getInstance();
        this.preferencias = new HashMap<>();
    }

    private Aluno(Map<Integer, Utilizador> utilizadores, Map<Integer, List<Horario>> horariosMap, Map<Integer, Turno> turnos, Map<Integer, TurnoPreferencia> preferencias){
        this.utilizadores = utilizadores;
        this.horariosMap = horariosMap;
        this.turnos = turnos;
        this.preferencias = preferencias;
    }

    public int getID(){
        return id;
    }

    public void setID(int id){
        this.id = id;
    }

    public String getNome(){
        return Nome;
    }

    public void setNome(String Nome){
        this.Nome = Nome;
    }

    public String getEstatuto(){
        return Estatuto;
    }

    public void setEstatuto(String Estatuto){
        this.Estatuto = Estatuto;
    }

    public int getAno_inscricao(){
        return Ano_Inscricao;
    }

    public void setAno_inscricao(int Ano){
        this.Ano_Inscricao = Ano;
    }

    public int getMedia_Curso(){
        return Media_Curso;
    }

    public void setMedia_Curso(int Media_Curso){
        this.Media_Curso = Media_Curso;
    }

    private boolean verificaPreferencias(Turno turno, Aluno aluno) {
        TurnoPreferencia pref = preferencias.get(turno.getIdTurno());
        if (pref == null) return true;

        switch (pref.getTipoPreferencia()) {
            case "ESTATUTO_ESPECIAL":
                if (!aluno.getEstatuto().equals(pref.getValorPreferencia())) {
                    return false;
                }
                break;
            case "MEDIA_MINIMA":
                if (aluno.getMedia_Curso() < Double.parseDouble(pref.getValorPreferencia())) {
                    return false;
                }
                break;
        }
        return true;
    }

    public List<Horario> consultaHorario(int Numero_id) {
        if (!utilizadores.containsKey(Numero_id)) {
            throw new IllegalArgumentException("Utilizador não encontrado");
        }
        return horariosMap.getOrDefault(Numero_id, new ArrayList<>());
    }

    public List<Turno> getTurnosCompativeis(int numeroAluno, int turnoAtual) {
        Turno turnoOriginal = turnos.get(turnoAtual);
        if (!getUCsInscritas().stream().anyMatch(uc -> uc.getIdUC() == turnoOriginal.getCadeira())) {
            throw new IllegalArgumentException("Aluno não está inscrito na UC deste turno");
        }
        List<Horario> horariosAluno = horariosMap.get(numeroAluno);
        Aluno aluno = AlunoDAO.getInstance().get(numeroAluno);

        TurnoPreferencia prefAtual = preferencias.get(turnoAtual);
        boolean alunoAtendePrefAtual = prefAtual != null && verificaPreferencias(turnoOriginal, aluno);

        return turnos.values().stream()
                .filter(t -> t.getCapacidade() > 0)
                .filter(t -> t.getIdTurno() != turnoAtual)
                .filter(t -> t.getCadeira() == turnoOriginal.getCadeira())
                .filter(t -> t.isPratica() == turnoOriginal.isPratica())
                .filter(t -> {
                    if (alunoAtendePrefAtual) {
                        TurnoPreferencia prefNovo = preferencias.get(t.getIdTurno());
                        return prefNovo != null && verificaPreferencias(t, aluno);
                    }
                    TurnoPreferencia prefNovo = preferencias.get(t.getIdTurno());
                    return prefNovo == null || !verificaPreferencias(t, aluno);
                })
                .filter(t -> !horariosAluno.stream()
                        .map(h -> turnos.get(h.getTurno()))
                        .filter(ht -> ht.getIdTurno() != turnoAtual)
                        .anyMatch(ht -> utilities.hasTimeConflict(ht, t)))
                .collect(Collectors.toList());
    }

    public void trocarTurno(int numeroAluno, int turnoAtual, int novoTurno) {
        // Obter horários atuais do aluno
        List<Horario> horariosAluno = horariosMap.get(numeroAluno);
        if (horariosAluno == null) {
            throw new IllegalArgumentException("Aluno não tem horário registrado");
        }

        // Obter turnos e verificar validade
        Turno turnoNovo = turnos.get(novoTurno);
        Turno turnoAntigo = turnos.get(turnoAtual);
        if (turnoNovo == null || turnoNovo.getCapacidade() <= 0) {
            throw new IllegalArgumentException("Turno novo inválido ou sem vagas");
        }

        // Verificar se aluno está inscrito nas UCs dos turnos
        List<UC> ucsInscritas = getUCsInscritas();
        boolean inscritoTurnoAntigo = ucsInscritas.stream()
                .anyMatch(uc -> uc.getIdUC() == turnoAntigo.getCadeira());
        boolean inscritoTurnoNovo = ucsInscritas.stream()
                .anyMatch(uc -> uc.getIdUC() == turnoNovo.getCadeira());

        if (!inscritoTurnoAntigo || !inscritoTurnoNovo) {
            throw new IllegalArgumentException("Aluno não está inscrito em uma das UCs dos turnos");
        }

        // Verificar preferências
        Aluno aluno = AlunoDAO.getInstance().get(numeroAluno);
        TurnoPreferencia prefAtual = preferencias.get(turnoAtual);
        boolean alunoAtendePrefAtual = prefAtual != null && verificaPreferencias(turnoAntigo, aluno);

        if (alunoAtendePrefAtual) {
            TurnoPreferencia prefNovo = preferencias.get(novoTurno);
            if (prefNovo == null || !verificaPreferencias(turnoNovo, aluno)) {
                throw new IllegalArgumentException("Não é possível trocar de turno preferencial para turno não preferencial");
            }
        }

        // Realizar a troca
        horariosAluno.removeIf(h -> h.getTurno() == turnoAtual);
        turnoAntigo.setCapacidade(turnoAntigo.getCapacidade() + 1);
        turnoNovo.setCapacidade(turnoNovo.getCapacidade() - 1);
        horariosAluno.add(new Horario(numeroAluno, 1, novoTurno));

        // Atualizar os dados
        turnos.put(turnoAtual, turnoAntigo);
        turnos.put(novoTurno, turnoNovo);
        horariosMap.put(numeroAluno, horariosAluno);
    }


    public List<UC> getUCsInscritas() {
        Map<Integer, UC> ucs = ucDAO.getInstance();
        List<UC> ucsInscritas = new ArrayList<>();

        // Buscar inscrições do aluno
        Map<Integer, Inscricao> inscricoes = InscricaoUCDAO.getInstance();

        // Filtrar e mapear para UCs
        inscricoes.values().stream()
                .filter(inscricao -> inscricao.getAluno() == this.id)
                .map(inscricao -> ucs.get(inscricao.getUC()))
                .filter(uc -> uc != null)
                .forEach(ucsInscritas::add);

        return ucsInscritas;
    }

    public void inscreverUC(int ucId) {
        Map<Integer, Inscricao> inscricoes = InscricaoUCDAO.getInstance();

        // Verifica se o aluno já está inscrito
        if (inscricoes.values().stream()
                .anyMatch(i -> i.getAluno() == this.id && i.getUC() == ucId)) {
            throw new IllegalArgumentException("Já está inscrito nesta UC");
        }

        // Verifica se o aluno tem estatuto especial
        if (!this.Estatuto.isEmpty() && getUCsInscritas().size() >= 6) {
            throw new IllegalArgumentException("Alunos com estatuto especial não podem inscrever-se em mais de 4 UCs");
        }

        // Criar e guardar a nova inscrição
        int chave = this.id * 1000 + ucId; // chave única para a inscrição
        Inscricao inscricao = new Inscricao(this.id, ucId);
        inscricoes.put(chave, inscricao);
    }
}
