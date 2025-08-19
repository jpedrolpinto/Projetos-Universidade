package data;

import java.sql.*;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

import business.utilizadores.Regente;

public class RegenteDAO implements Map<Integer, Regente> {
    private static RegenteDAO singleton = null;

    private RegenteDAO() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            String sql = "CREATE TABLE IF NOT EXISTS `HorarioSim`.`Regente_Curso` (" +
                    "id_Regente INT NOT NULL," +
                    "NomeRegente VARCHAR(45) NOT NULL, " +
                    "FOREIGN KEY (id_Regente) REFERENCES Utilizador(Numero_id)," +
                    "PRIMARY KEY (id_Regente))";
            stm.executeUpdate(sql);

        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    public static RegenteDAO getInstance() {
        if (RegenteDAO.singleton == null) {
            RegenteDAO.singleton = new RegenteDAO();
        }
        return RegenteDAO.singleton;
    }

    @Override
    public void clear() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            stm.executeUpdate("TRUNCATE Regente_Curso");
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
             ResultSet rs = stm.executeQuery("SELECT id_Regente FROM `HorarioSim`.`Regente_Curso` WHERE id_Regente=" + key)) {
            r = rs.next();
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return r;
    }

    @Override
    public boolean containsValue(Object value) {
        Regente c = (Regente) value;
        return this.containsKey(c.getId_Regente());
    }

    @Override
    public Set<Entry<Integer, Regente>> entrySet() {
        return this.keySet().stream().map(k -> Map.entry(k, this.get(k))).collect(Collectors.toSet());
    }

    @Override
    public Regente get(Object key) {
        Regente r = null;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`Regente_Curso` WHERE id_Regente=" + key)) {
            if (rs.next()) {
                r = new Regente(
                        rs.getInt("id_Regente"),
                        rs.getString("NomeRegente")
                );
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return r;
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
             ResultSet rs = stm.executeQuery("SELECT id_Regente FROM Regente_Curso")) {
            while (rs.next()) {
                int idu = rs.getInt("id_Regente");
                res.add(idu);
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return res;
    }

    @Override
    public Regente put(Integer key, Regente value) {
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm2 = conn.prepareStatement("INSERT INTO Regente_Curso (id_Regente,NomeRegente) VALUES (?,?)")) {
                pstm2.setInt(1, value.getId_Regente());
                pstm2.setString(2, value.getNome_Regente());
                pstm2.executeUpdate();
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return value;
    }

    @Override
    public void putAll(Map<? extends Integer, ? extends Regente> m) {
        m.keySet().forEach(i -> this.put(i, m.get(i)));
    }

    @Override
    public Regente remove(Object key) {
        Regente value = null;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM Regente_Curso WHERE id_Regente = ?")) {
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
             ResultSet rs = stm.executeQuery("SELECT count(*) FROM `HorarioSim`.`Regente_Curso`")) {
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
    public Collection<Regente> values() {
        return this.keySet().stream().map(this::get).collect(Collectors.toList());
    }
}
