package business.horario;

import business.uni.UC;
import data.HorarioDAOWraper;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Turno {
    private int idTurno;
    private String Sala;
    private String Hora;
    private String DataT;
    private int Capacidade;
    private boolean Pratica;
    private boolean Teorica;
    private int Cadeira;


    public Turno(int idTurno){
        this.idTurno = idTurno;
        this.Sala = "";
        this.Hora = "";
        this.DataT = "";
        this.Capacidade = 0;
        this.Pratica = false;
        this.Teorica = false;
        this.Cadeira = 0;
    }

    public Turno(int idTurno, String Sala, String Hora, String DataT, int Capacidade, boolean Pratica, boolean Teorica, int Cadeira){
        this.idTurno = idTurno;
        this.Sala = Sala;
        this.Hora = Hora;
        this.DataT = DataT;
        this.Capacidade = Capacidade;
        this.Pratica = Pratica;
        this.Teorica = Teorica;
        this.Cadeira = Cadeira;
    }

    public Turno(){}

    public int getIdTurno() {
        return idTurno;
    }

    public void setIdTurno(int idTurno) {
        this.idTurno = idTurno;
    }

    public String getSala() {
        return Sala;
    }

    public void setSala(String sala) {
        Sala = sala;
    }

    public String getHora() {
        return Hora;
    }

    public void setHora(String hora) {
        Hora = hora;
    }

    public String getDataT() {
        return DataT;
    }

    public void setDataT(String dataT) {
        DataT = dataT;
    }

    public int getCapacidade() {
        return Capacidade;
    }

    public void setCapacidade(int capacidade) {
        Capacidade = capacidade;
    }

    public boolean isPratica() {
        return Pratica;
    }

    public void setPratica(boolean pratica) {
        Pratica = pratica;
    }

    public boolean isTeorica() {
        return Teorica;
    }

    public void setTeorica(boolean teorica) {
        Teorica = teorica;
    }

    public int getCadeira() {
        return Cadeira;
    }

    public void setCadeira(int cadeira) {
        Cadeira = cadeira;
    }

    public List<Turno> importaTurnos(Map<Integer, Turno> turnos) {
        // Retorna todos os turnos sem filtrar por capacidade
        return new ArrayList<>(turnos.values());
    }

    public boolean criaTurno(String sala, String hora, String dia, int capacidade,
                             boolean pratica, int ucId,
                             Map<Integer, Turno> turnos, Map<Integer, UC> ucs) {
        if (!ucs.containsKey(ucId)) {
            throw new IllegalArgumentException("UC não existe");
        }

        if (!pratica) {
            long turnosTeoricos = turnos.values().stream()
                    .filter(t -> t.getCadeira() == ucId)
                    .filter(Turno::isTeorica)
                    .count();
            if (turnosTeoricos >= 2) {
                throw new IllegalArgumentException("Já existem 2 turnos teóricos para esta UC");
            }
        }

        boolean salaOcupada = turnos.values().stream()
                .filter(t -> t.getDataT().equals(dia))
                .filter(t -> t.getHora().equals(hora))
                .anyMatch(t -> t.getSala().equals(sala));

        if (salaOcupada) {
            throw new IllegalArgumentException("Sala já ocupada neste horário");
        }

        int novoId = turnos.keySet().stream()
                .mapToInt(Integer::intValue)
                .max()
                .orElse(0) + 1;

        if (novoId < 10) {
            novoId = Integer.parseInt("0" + novoId);
        }

        Turno novoTurno = new Turno(
                novoId, sala, hora, dia, capacidade, pratica, !pratica, ucId
        );

        turnos.put(novoId, novoTurno);
        return true;
    }

    public Map<Integer, Integer> getOcupacaoTurnos(Map<Integer, Turno> turnos, Map<Integer, List<Horario>> horarios) {
        Map<Integer, Integer> ocupacao = new HashMap<>();

        // Inicializa ocupação com a capacidade atual de cada turno
        turnos.values().forEach(turno -> {
            ocupacao.put(turno.getIdTurno(), turno.getCapacidade());
        });

        return ocupacao;
    }

}
