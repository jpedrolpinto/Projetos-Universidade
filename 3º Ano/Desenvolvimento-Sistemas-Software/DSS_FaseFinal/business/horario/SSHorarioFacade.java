package business.horario;

import business.uni.UC;
import business.utilizadores.Aluno;
import business.utilizadores.Regente;
import business.utilizadores.Utilizador;
import data.*;

import java.util.*;

public class SSHorarioFacade implements ISSHorario {
    private final Map<Integer, Utilizador> utilizadores;
    private final Map<Integer, List<Horario>> horarios;
    private final Map<Integer, Turno> turnos;
    private final Map<Integer, UC> ucs;
    private final Map<Integer, Regente> regentes;
    private final Horario horarioManager;
    private final Turno turnoManager;

    public SSHorarioFacade() {
        this.utilizadores = UtilizadoresDAO.getInstance();
        this.horarios = HorarioDAOWraper.getInstance();
        this.turnos = TurnoDAO.getInstance();
        this.ucs = ucDAO.getInstance();
        this.regentes = RegenteDAO.getInstance();
        this.horarioManager = new Horario();
        this.turnoManager = new Turno();
    }

    public Turno getTurno(int idTurno) {
        return turnos.get(idTurno);
    }

    public UC getUC(int idUC) {
        return ucs.get(idUC);
    }

    public List<UC> getUCs() {
        return new ArrayList<>(ucs.values());
    }

    public String getRegenteName(int id) {
        Regente regente = regentes.get(id);
        return regente != null ? regente.getNome_Regente() : "Desconhecido";
    }

    @Override
    public List<Turno> importaTurnos() {
        return turnoManager.importaTurnos(turnos);
    }

    @Override
    public List<Aluno> importaAlunos() {
        return horarioManager.importaAlunos(utilizadores);
    }

    @Override
    public boolean criaTurno(String sala, String hora, String dia, int capacidade, boolean pratica, int ucId) {
        return turnoManager.criaTurno(sala, hora, dia, capacidade, pratica, ucId, turnos, ucs);
    }

    @Override
    public Map<Integer, Integer> getOcupacaoTurnos() {
        return turnoManager.getOcupacaoTurnos(turnos, horarios);
    }
}
