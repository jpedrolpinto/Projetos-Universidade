package data;

import java.sql.*;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

import business.horario.Horario;

public class HorarioDAO implements Map<Integer, Horario> {
    private static HorarioDAO singleton = null;

    private HorarioDAO() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            String sql = "CREATE TABLE IF NOT EXISTS `HorarioSim`.`Horario` ("+
                    "Aluno INT NOT NULL,"+
                    "Regente INT NOT NULL,"+
                    "Turno INT NOT NULL,"+
                    "FOREIGN KEY (Aluno) REFERENCES Aluno(id),"+
                    "FOREIGN KEY (Regente) REFERENCES Regente_Curso(id_Regente),"+
                    "FOREIGN KEY (Turno) REFERENCES Turno(idTurno),"+
                    "PRIMARY KEY (Aluno, Turno))";
            stm.executeUpdate(sql);

        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    public static HorarioDAO getInstance() {
        if (HorarioDAO.singleton == null) {
            HorarioDAO.singleton = new HorarioDAO();
        }
        return HorarioDAO.singleton;
    }

    @Override
    public void clear() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            stm.executeUpdate("TRUNCATE Horario");
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
             ResultSet rs = stm.executeQuery("SELECT Aluno FROM `HorarioSim`.`Horario` WHERE Aluno=" + key)) {
            r = rs.next();
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return r;
    }

    @Override
    public boolean containsValue(Object value) {
        Horario c = (Horario) value;
        return this.containsKey(c.getAluno());
    }

    @Override
    public Set<Entry<Integer, Horario>> entrySet() {
        return this.keySet().stream().map(k -> Map.entry(k, this.get(k))).collect(Collectors.toSet());
    }

    @Override
    public Horario get(Object key) {
        Horario c = null;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`Horario` WHERE Aluno=" + key)) {
            c = rs.next() ? new Horario(rs.getInt("Aluno"), rs.getInt("Regente"), rs.getInt("Turno")) : null;
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return c;
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
             ResultSet rs = stm.executeQuery("SELECT Aluno FROM Horario")) {
            while (rs.next()) {
                int idu = rs.getInt("Aluno");
                res.add(idu);
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return res;
    }

    @Override
    public Horario put(Integer key, Horario value) {
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm2 = conn.prepareStatement("INSERT IGNORE INTO Horario (Aluno,Regente,Turno) VALUES (?,?,?)")) {
                pstm2.setInt(1, value.getAluno());
                pstm2.setInt(2, value.getRegente());
                pstm2.setInt(3, value.getTurno());

                pstm2.executeUpdate();
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return value;
    }

    @Override
    public void putAll(Map<? extends Integer, ? extends Horario> m) {
        m.keySet().forEach(i -> this.put(i, m.get(i)));
    }

    @Override
    public Horario remove(Object key) {
        Horario value = null;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM Horario WHERE Aluno = ?")) {
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
             ResultSet rs = stm.executeQuery("SELECT count(*) FROM `HorarioSim`.`Horario`")) {
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
    public Collection<Horario> values() {
        return this.keySet().stream().map(this::get).collect(Collectors.toList());
    }
}


