package business.utilizadores;

public class Utilizador {
    private int Numero_id;
    private String Pass;
    private String Email;
    private boolean isRegente;

    public Utilizador(int Numero_id, String Pass){
        this.Numero_id = Numero_id;
        this.Pass = Pass;
        this.Email = "";
        this.isRegente = false;
    }

    public Utilizador(int Numero_id, String Password, String Email, boolean isRegente){
        this.Numero_id = Numero_id;
        this.Pass = Password;
        this.Email = Email;
        this.isRegente = isRegente;
    }

    Utilizador(){}

    public int getNumeroID(){
        return Numero_id;
    }

    public void setNumero_id(int Numero_id){
        this.Numero_id = Numero_id;
    }

    public String getPass(){
        return Pass;
    }

    public void setPass(String Pass){
        this.Pass = Pass;
    }

    public String getEmail(){
        return Email;
    }

    public void setEmail(String Email) {
        this.Email = Email;
    }

    public boolean isRegente() {
        return isRegente;
    }

    public void setRegente(boolean regente) {
        isRegente = regente;
    }


}
