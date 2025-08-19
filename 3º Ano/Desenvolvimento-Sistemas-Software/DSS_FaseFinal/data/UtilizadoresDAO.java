package data;

import java.sql.*;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

import business.utilizadores.Utilizador;

public class UtilizadoresDAO implements Map<Integer, Utilizador> {
    private static UtilizadoresDAO singleton = null;

    private UtilizadoresDAO() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            String sql = "CREATE TABLE IF NOT EXISTS `HorarioSim`.`Utilizador` (" +
                    "Numero_id INT NOT NULL," +
                    "Pass VARCHAR(10) NOT NULL," +
                    "Email VARCHAR(45) NOT NULL," +
                    "isRegente INT NOT NULL," +
                    "PRIMARY KEY (Numero_id))";
            stm.executeUpdate(sql);

        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    public static UtilizadoresDAO getInstance() {
        if (UtilizadoresDAO.singleton == null) {
            UtilizadoresDAO.singleton = new UtilizadoresDAO();
        }
        return UtilizadoresDAO.singleton;
    }

    @Override
    public void clear() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            stm.executeUpdate("TRUNCATE Utilizador");
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
             ResultSet rs = stm.executeQuery("SELECT Numero_id FROM `HorarioSim`.`Utilizador` WHERE Numero_id=" + key)) {
            r = rs.next();
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return r;
    }

    @Override
    public boolean containsValue(Object value) {
        Utilizador c = (Utilizador) value;
        return this.containsKey(c.getNumeroID());
    }

    @Override
    public Set<Entry<Integer, Utilizador>> entrySet() {
        return this.keySet().stream().map(k -> Map.entry(k, this.get(k))).collect(Collectors.toSet());
    }

    @Override
    public Utilizador get(Object key) {
        Utilizador r = null;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`Utilizador` WHERE Numero_id=" + key)) {
            r = rs.next() ? new Utilizador(rs.getInt("Numero_id"), rs.getString("Pass"), rs.getString("Email"), rs.getInt("isRegente") == 1 ? true : false) : null;
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
             ResultSet rs = stm.executeQuery("SELECT Numero_id FROM Utilizador")) {
            while (rs.next()) {
                int idu = rs.getInt("Numero_id");
                res.add(idu);
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return res;
    }

    @Override
    public Utilizador put(Integer key, Utilizador value) {
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            if (this.containsKey(key)) {
                try (PreparedStatement pstm1 = conn.prepareStatement("UPDATE Utilizador SET Pass = ? WHERE Numero_id = ?")) {
                    pstm1.setString(1, value.getPass());
                    pstm1.setInt(2, key);
                    pstm1.executeUpdate();
                }
            } else {
                try (PreparedStatement pstm2 = conn.prepareStatement("INSERT INTO Utilizador (Numero_id,Pass,Email,isRegente) VALUES (?,?,?,?)")) {
                    pstm2.setInt(1, value.getNumeroID());
                    pstm2.setString(2, value.getPass());
                    pstm2.setString(3, value.getEmail());
                    pstm2.setInt(4, value.isRegente() ? 1 : 0);
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
    public void putAll(Map<? extends Integer, ? extends Utilizador> m) {
        m.keySet().forEach(i -> this.put(i, m.get(i)));
    }

    @Override
    public Utilizador remove(Object key) {
        Utilizador value = null;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM Utilizador WHERE Numero_id = ?")) {
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
             ResultSet rs = stm.executeQuery("SELECT count(*) FROM `HorarioSim`.`Utilizador`")) {
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
    public Collection<Utilizador> values() {
        return this.keySet().stream().map(this::get).collect(Collectors.toList());
    }
}
