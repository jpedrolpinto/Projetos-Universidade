package business.uni;

public class Inscricao {
    private int Aluno;
    private int UC;

    public Inscricao(int Aluno, int UC){
        this.Aluno = Aluno;
        this.UC = UC;
    }

    Inscricao(){}

    public int getAluno() {
        return Aluno;
    }

    public void setAluno(int aluno) {
        Aluno = aluno;
    }

    public int getUC() {
        return UC;
    }

    public void setUC(int UC) {
        this.UC = UC;
    }

}
