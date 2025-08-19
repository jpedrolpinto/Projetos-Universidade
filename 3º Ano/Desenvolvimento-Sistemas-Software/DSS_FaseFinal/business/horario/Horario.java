package business.horario;

import business.utilizadores.*;
import data.AlunoDAO;

import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

public class Horario {
    private int Aluno;
    private int Regente;
    private int Turno;

    public Horario(int Aluno, int Regente, int Turno){
        this.Aluno = Aluno;
        this.Regente = Regente;
        this.Turno = Turno;
    }

    Horario(){}

    public int getAluno(){
        return Aluno;
    }

    public void setAluno(int Aluno){
        this.Aluno = Aluno;
    }

    public int getRegente(){
        return Regente;
    }

    public void setRegente(int Regente){
        this.Regente = Regente;
    }

    public int getTurno(){
        return Turno;
    }

    public void setTurno(int Turno){
        this.Turno = Turno;
    }

    public List<Aluno> importaAlunos(Map<Integer, Utilizador> utilizadores) {
        List<Integer> alunosIds = utilizadores.values().stream()
                .filter(u -> !u.isRegente())
                .map(Utilizador::getNumeroID)
                .collect(Collectors.toList());

        Map<Integer, Aluno> alunos = AlunoDAO.getInstance();

        return alunos.values().stream()
                .filter(a -> alunosIds.contains(a.getID()))
                .sorted(Comparator.comparingInt(business.utilizadores.Aluno::getID))
                .collect(Collectors.toList());
    }

}

