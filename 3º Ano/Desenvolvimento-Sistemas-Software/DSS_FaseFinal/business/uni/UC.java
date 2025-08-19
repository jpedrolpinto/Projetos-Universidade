package business.uni;

public class UC {
    private int idUC;
    private String NomeUC;
    private int Ano;
    private int Semestre;
    private int Curso;

    public UC(int idUC){
        this.idUC = idUC;
        this.NomeUC = "";
        this.Ano = 0;
        this.Semestre = 0;
        this.Curso = 0;
    }

    public UC(int idUC, String NomeUC, int Ano, int Semestre, int Curso){
        this.idUC = idUC;
        this.NomeUC = NomeUC;
        this.Ano = Ano;
        this.Semestre = Semestre;
        this.Curso = Curso;
    }

    private UC(){}

    public int getIdUC() {
        return idUC;
    }

    public void setIdUC(int idUC) {
        this.idUC = idUC;
    }

    public String getNomeUC() {
        return NomeUC;
    }

    public void setNomeUC(String nomeUC) {
        NomeUC = nomeUC;
    }

    public int getAno() {
        return Ano;
    }

    public void setAno(int ano) {
        Ano = ano;
    }

    public int getSemestre() {
        return Semestre;
    }

    public void setSemestre(int semestre) {
        Semestre = semestre;
    }

    public int getCurso() {
        return Curso;
    }

    public void setCurso(int curso) {
        Curso = curso;
    }
}

