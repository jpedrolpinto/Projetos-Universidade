package business.utilizadores;

import business.horario.Horario;
import business.horario.TurnoPreferencia;
import business.horario.Turno;
import business.uni.UC;
import data.*;

import java.util.*;

public class SSUtilizadoresFacade implements ISSUtilizadores{
    private final Map<Integer, Utilizador> utilizadores;
    private final Map<Integer, List<Horario>> horarios;
    private final Map<Integer, TurnoPreferencia> preferencias;

    public SSUtilizadoresFacade() {
        this.utilizadores = UtilizadoresDAO.getInstance();
        this.horarios = HorarioDAOWraper.getInstance();
        this.preferencias = new HashMap<>();
    }

    private Regente getRegenteFromUtilizador(int regenteId) {
        Utilizador user = this.utilizadores.get(regenteId);
        if (user == null || !user.isRegente()) {
            throw new IllegalStateException("Utilizador não é um regente");
        }

        // Em vez de tentar converter o Utilizador, buscamos diretamente do RegenteDAO
        Regente regente = RegenteDAO.getInstance().get(regenteId);
        if (regente == null) {
            throw new IllegalStateException("Regente não encontrado");
        }
        return regente;
    }

    @Override
    public boolean existeUtilizador(int Numero_id) {
        return this.utilizadores.containsKey(Numero_id);
    }

    @Override
    public void registaUtilizador(int Numero_id, String Pass, String Nome, String Email, String Estatuto, int Media, int Ano) {
        // Cria o aluno com o ID gerado e o ano
        Aluno aluno = new Aluno(Numero_id, Nome, Estatuto, Ano, Media);

        // Cria o utilizador base com as credenciais
        Utilizador utilizador = new Utilizador(Numero_id, Pass, Email, false);

        // Guarda nas estruturas de dados
        this.utilizadores.put(Numero_id, utilizador);
        AlunoDAO.getInstance().put(Numero_id, aluno);
    }

    @Override
    public boolean validaPassword(int Numero_id, String Pass) {
        Utilizador user = this.utilizadores.get(Numero_id);
        return user != null && user.getPass().equals(Pass);
    }

    @Override
    public Utilizador getUtilizador(int Numero_id) {
        return this.utilizadores.get(Numero_id);
    }

    @Override
    public void definirPreferenciaTurno(int regenteId, int idTurno, String tipoPreferencia, String valorPreferencia) {
        Regente regente = getRegenteFromUtilizador(regenteId);
        regente.definirPreferenciaTurno(idTurno, tipoPreferencia, valorPreferencia);
    }

    @Override
    public Map<Integer, String> alocaTodosAlunos(int regenteId) {
        Regente regente = getRegenteFromUtilizador(regenteId);
        return regente.alocaTodosAlunos();
    }

    @Override
    public void alocaManual(int regenteId, int numeroAluno, int turnoId) {
        Regente regente = getRegenteFromUtilizador(regenteId);
        regente.alocaManual(numeroAluno, turnoId);
    }

    @Override
    public List<Horario> consultaHorario(int Numero_id) {
        Aluno aluno = AlunoDAO.getInstance().get(Numero_id);
        if (aluno == null) {
            throw new IllegalStateException("Aluno não encontrado");
        }
        return aluno.consultaHorario(Numero_id);
    }

    @Override
    public List<Turno> getTurnosCompativeis(int numeroAluno, int turnoAtual) {
        Aluno aluno = AlunoDAO.getInstance().get(numeroAluno);
        if (aluno == null) {
            throw new IllegalStateException("Aluno não encontrado");
        }
        return aluno.getTurnosCompativeis(numeroAluno, turnoAtual);
    }

    @Override
    public void trocarTurno(int numeroAluno, int turnoAtual, int novoTurno) {
        Aluno aluno = AlunoDAO.getInstance().get(numeroAluno);
        if (aluno == null) {
            throw new IllegalStateException("Aluno não encontrado");
        }
        aluno.trocarTurno(numeroAluno, turnoAtual, novoTurno);
    }

    @Override
    public void inscreverUC(int numeroAluno, int ucId) {
        Aluno aluno = AlunoDAO.getInstance().get(numeroAluno);
        if (aluno == null) {
            throw new IllegalStateException("Aluno não encontrado");
        }
        aluno.inscreverUC(ucId);
    }

    @Override
    public List<UC> consultarUCsInscritas(int numeroAluno) {
        Aluno aluno = AlunoDAO.getInstance().get(numeroAluno);
        if (aluno == null) {
            throw new IllegalStateException("Aluno não encontrado");
        }
        return aluno.getUCsInscritas();
    }

}
