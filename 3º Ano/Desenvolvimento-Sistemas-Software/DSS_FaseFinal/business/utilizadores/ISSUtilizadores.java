package business.utilizadores;

import business.horario.*;
import business.uni.*;

import java.util.List;
import java.util.Map;

public interface ISSUtilizadores {

    boolean existeUtilizador(int Numero_id);

    void registaUtilizador(int Numero_id, String Pass, String Nome, String Email, String Estatuto, int Media, int Ano);

    boolean validaPassword(int Numero_id, String Pass);

    Utilizador getUtilizador(int Numero_id);

    void definirPreferenciaTurno(int regenteId, int idTurno, String tipoPreferencia, String valorPreferencia);

    Map<Integer, String> alocaTodosAlunos(int regenteId);

    void alocaManual(int regenteId, int numeroAluno, int turnoId);

    List<Horario> consultaHorario(int Numero_id);

    List<Turno> getTurnosCompativeis(int numeroAluno, int turnoAtual);

    void trocarTurno(int numeroAluno, int turnoAtual, int novoTurno);

    List<UC> consultarUCsInscritas(int numeroAluno);

    void inscreverUC(int numeroAluno, int ucId);

}
