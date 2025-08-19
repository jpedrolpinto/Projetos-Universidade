package business.horario;

public class TurnoPreferencia {
    private int idTurno;
    private int idRegente;
    private String tipoPreferencia;  // "ESTATUTO_ESPECIAL", "MEDIA_ALTA", etc.
    private String valorPreferencia; // "Especial", "15.0", etc.

    public TurnoPreferencia(int idRegente, int idTurno, String tipoPreferencia, String valorPreferencia) {
        this.idRegente = idRegente;
        this.idTurno = idTurno;
        this.tipoPreferencia = tipoPreferencia;
        this.valorPreferencia = valorPreferencia;
    }


    public int getIdTurno() {
        return idTurno;
    }

    public void setIdTurno(int idTurno) {
        this.idTurno = idTurno;
    }

    public int getIdRegente() {
        return idRegente;
    }

    public void setIdRegente(int idRegente) {
        this.idRegente = idRegente;
    }

    public String getTipoPreferencia() {
        return tipoPreferencia;
    }

    public void setTipoPreferencia(String tipoPreferencia) {
        this.tipoPreferencia = tipoPreferencia;
    }

    public String getValorPreferencia() {
        return valorPreferencia;
    }

    public void setValorPreferencia(String valorPreferencia) {
        this.valorPreferencia = valorPreferencia;
    }
}
