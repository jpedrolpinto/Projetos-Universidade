package data;

import java.sql.*;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

import business.uni.UC;

public class ucDAO implements Map<Integer, UC> {
    private static ucDAO singleton = null;

    private ucDAO() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            String sql = "CREATE TABLE IF NOT EXISTS `HorarioSim`.`UC` ("+
                    "idUC INT NOT NULL,"+
                    "NomeUC VARCHAR(45) NOT NULL,"+
                    "Ano INT NOT NULL," +
                    "Semestre INT NOT NULL,"+
                    "Curso INT NOT NULL,"+
                    "FOREIGN KEY (Semestre) REFERENCES Semestre(Numero_Semestre),"+
                    "FOREIGN KEY (Curso) REFERENCES Curso(CodigoCurso),"+
                    "PRIMARY KEY (idUC))";
            stm.executeUpdate(sql);
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    public static ucDAO getInstance() {
        if (ucDAO.singleton == null) {
            ucDAO.singleton = new ucDAO();
        }
        return ucDAO.singleton;
    }

    @Override
    public void clear() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            stm.executeUpdate("TRUNCATE UC");
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
             ResultSet rs = stm.executeQuery("SELECT idUC FROM `HorarioSim`.`UC` WHERE idUC=" + key)) {
            r = rs.next();
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return r;
    }

    @Override
    public boolean containsValue(Object value) {
        UC uc = (UC) value;
        return this.containsKey(uc.getIdUC());
    }

    @Override
    public Set<Entry<Integer, UC>> entrySet() {
        return this.keySet().stream().map(k -> Map.entry(k, this.get(k))).collect(Collectors.toSet());
    }

    @Override
    public UC get(Object key) {
        UC uc = null;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`UC` WHERE idUC=" + key)) {
            uc = rs.next() ? new UC(rs.getInt("idUC"), rs.getString("NomeUC"), rs.getInt("Ano"),  rs.getInt("Semestre"), rs.getInt("Curso")) : null;
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return uc;
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
             ResultSet rs = stm.executeQuery("SELECT idUC FROM UC")) {
            while (rs.next()) {
                int idu = rs.getInt("idUC");
                res.add(idu);
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return res;
    }

    @Override
    public UC put(Integer key, UC value) {
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm2 = conn.prepareStatement("INSERT INTO UC (idUC,NomeUC,Ano,Semestre,Curso) VALUES (?,?,?,?,?)")) {
                pstm2.setInt(1, value.getIdUC());
                pstm2.setString(2, value.getNomeUC());
                pstm2.setInt(3, value.getAno());
                pstm2.setInt(4, value.getSemestre());
                pstm2.setInt(5, value.getCurso());
                pstm2.executeUpdate();
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return value;
    }

    @Override
    public void putAll(Map<? extends Integer, ? extends UC> m) {
        m.keySet().forEach(i -> this.put(i, m.get(i)));
    }

    @Override
    public UC remove(Object key) {
        UC value = null;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM UC WHERE idUC = ?")) {
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
             ResultSet rs = stm.executeQuery("SELECT count(*) FROM `HorarioSim`.`UC`")) {
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
    public Collection<UC> values() {
        return this.keySet().stream().map(this::get).collect(Collectors.toList());
    }
}


