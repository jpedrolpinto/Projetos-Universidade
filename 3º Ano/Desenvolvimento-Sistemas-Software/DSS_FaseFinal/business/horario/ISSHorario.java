package business.horario;

import business.uni.UC;
import business.utilizadores.Aluno;

import java.util.List;
import java.util.Map;

public interface ISSHorario {

    List<Turno> importaTurnos();

    List<Aluno> importaAlunos();

    boolean criaTurno(String sala, String hora, String dia, int capacidade, boolean pratica, int ucId);

    Turno getTurno(int turno);
    UC getUC(int idUC);
    List<UC> getUCs();
    String getRegenteName(int id);

    Map<Integer, Integer> getOcupacaoTurnos();
}
