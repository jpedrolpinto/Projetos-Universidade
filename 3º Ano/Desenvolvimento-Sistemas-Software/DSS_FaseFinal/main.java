import UI.GestHorariosUI;

public class main {
    public static void main(String[] args){
        try{
            new GestHorariosUI().run();
        }
        catch (Exception e){
            System.out.println("Nao arrancou direito: " + e.getMessage());
        }
    }
}
