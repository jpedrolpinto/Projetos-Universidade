package data;

import java.sql.*;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

import business.utilizadores.Aluno;

public class AlunoDAO implements Map<Integer, Aluno> {
    private static AlunoDAO singleton = null;

    private AlunoDAO() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            String sql = "CREATE TABLE IF NOT EXISTS `HorarioSim`.`Aluno` (" +
                    "id INT NOT NULL," +
                    "Nome VARCHAR(45) NOT NULL, " +
                    "Estatuto VARCHAR(45) NOT NULL," +
                    "Ano_Incricao INT NOT NULL," + // Mantido igual ao SQL
                    "Media_Curso INT NOT NULL," +
                    "FOREIGN KEY (id) REFERENCES Utilizador(Numero_id)," +
                    "PRIMARY KEY (id))";
            stm.executeUpdate(sql);

        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    public static AlunoDAO getInstance() {
        if (AlunoDAO.singleton == null) {
            AlunoDAO.singleton = new AlunoDAO();
        }
        return AlunoDAO.singleton;
    }

    @Override
    public void clear() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            stm.executeUpdate("TRUNCATE Aluno");
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    @Override
    public boolean containsKey(Object key) {
        boolean r;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT id FROM `HorarioSim`.`Aluno` WHERE id=" + key)) {
            r = rs.next();
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return r;
    }

    @Override
    public boolean containsValue(Object value) {
        Aluno c = (Aluno) value;
        return this.containsKey(c.getID());
    }

    @Override
    public Set<Entry<Integer, Aluno>> entrySet() {
        return this.keySet().stream().map(k -> Map.entry(k, this.get(k))).collect(Collectors.toSet());
    }

    @Override
    public Aluno get(Object key) {
        Aluno a = null;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`Aluno` WHERE id=" + key)) {
            a = rs.next() ? new Aluno(
                    rs.getInt("id"),
                    rs.getString("Nome"),
                    rs.getString("Estatuto"),
                    rs.getInt("Ano_Incricao"),
                    rs.getInt("Media_Curso")
            ) : null;
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return a;
    }

    @Override
    public boolean isEmpty() {
        return this.size() == 0;
    }

    @Override
    public Set<Integer> keySet() {
        Set<Integer> res = new HashSet<>();
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT id FROM Aluno")) {
            while (rs.next()) {
                int idu = rs.getInt("id");
                res.add(idu);
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return res;
    }

    @Override
    public Aluno put(Integer key, Aluno value) {
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            if (this.containsKey(key)) {
                try (PreparedStatement pstm1 = conn.prepareStatement("UPDATE Aluno SET Estatuto = ? WHERE id = ?")) {
                    pstm1.setString(1, value.getEstatuto());
                    pstm1.setInt(2, key);
                    pstm1.executeUpdate();
                }
            } else {
                try (PreparedStatement pstm2 = conn.prepareStatement("INSERT INTO Aluno (id,Nome,Estatuto,Ano_Incricao,Media_Curso) VALUES (?,?,?,?,?)")) {
                    pstm2.setInt(1, value.getID());
                    pstm2.setString(2, value.getNome());
                    pstm2.setString(3, value.getEstatuto());
                    pstm2.setInt(4, value.getAno_inscricao());
                    pstm2.setInt(5, value.getMedia_Curso());
                    pstm2.executeUpdate();
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return value;
    }

    @Override
    public void putAll(Map<? extends Integer, ? extends Aluno> m) {
        m.keySet().forEach(i -> this.put(i, m.get(i)));
    }

    @Override
    public Aluno remove(Object key) {
        Aluno value = null;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM Aluno WHERE id = ?")) {
                value = this.get(key);
                pstm.setInt(1, (Integer) key);
                pstm.executeUpdate();
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return value;
    }

    @Override
    public int size() {
        int i = 0;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement();
             ResultSet rs = stm.executeQuery("SELECT count(*) FROM `HorarioSim`.`Aluno`")) {
            if (rs.next()) {
                i = rs.getInt(1);
            }
        } catch (Exception e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return i;
    }

    @Override
    public Collection<Aluno> values() {
        return this.keySet().stream().map(this::get).collect(Collectors.toList());
    }
}
