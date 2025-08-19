package data;

import java.sql.*;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

import business.horario.Turno;

public class TurnoDAO implements Map<Integer, Turno> {
    private static TurnoDAO singleton = null;

    private TurnoDAO() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            String sql = "CREATE TABLE IF NOT EXISTS `HorarioSim`.`Turno` ("+
                    "idTurno INT NOT NULL,"+
                    "Sala VARCHAR(45) NOT NULL,"+
                    "Hora VARCHAR(5) NOT NULL,"+
                    "DataT VARCHAR(10) NOT NULL,"+
                    "Capacidade INT NOT NULL,"+
                    "Cadeira INT NOT NULL,"+
                    "Pratica INT NOT NULL,"+
                    "Teorica INT NOT NULL,"+
                    "FOREIGN KEY (Cadeira) REFERENCES UC(idUC),"+
                    "PRIMARY KEY (idTurno))";
            stm.executeUpdate(sql);
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    public static TurnoDAO getInstance() {
        if (TurnoDAO.singleton == null) {
            TurnoDAO.singleton = new TurnoDAO();
        }
        return TurnoDAO.singleton;
    }

    @Override
    public void clear() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            stm.executeUpdate("TRUNCATE Turno");
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
             ResultSet rs = stm.executeQuery("SELECT idTurno FROM `HorarioSim`.`Turno` WHERE idTurno=" + key)) {
            r = rs.next();
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return r;
    }

    @Override
    public boolean containsValue(Object value) {
        Turno t = (Turno) value;
        return this.containsKey(t.getIdTurno());
    }

    @Override
    public Set<Entry<Integer, Turno>> entrySet() {
        return this.keySet().stream().map(k -> Map.entry(k, this.get(k))).collect(Collectors.toSet());
    }

    @Override
    public Turno get(Object key) {
        Turno t = null;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`Turno` WHERE idTurno=" + key)) {
            t = rs.next() ? new Turno(rs.getInt("idTurno"), rs.getString("Sala"), rs.getString("Hora"), rs.getString("DataT"), rs.getInt("Capacidade"), rs.getInt("Pratica") == 1 ? true : false, rs.getInt("Teorica") == 1 ? true : false, rs.getInt("Cadeira")) : null;
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return t;
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
             ResultSet rs = stm.executeQuery("SELECT idTurno FROM Turno")) { // Era "FROM Aluno"
            while (rs.next()) {
                int idu = rs.getInt("idTurno");
                res.add(idu);
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return res;
    }

    @Override
    public Turno put(Integer key, Turno value) {
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD)) {
            if (this.containsKey(key)) {
                // Update existing turno
                try (PreparedStatement pstm = conn.prepareStatement(
                        "UPDATE Turno SET Sala = ?, Hora = ?, DataT = ?, " +
                                "Capacidade = ?, Cadeira = ?, Pratica = ?, Teorica = ? " +
                                "WHERE idTurno = ?")) {
                    pstm.setString(1, value.getSala());
                    pstm.setString(2, value.getHora());
                    pstm.setString(3, value.getDataT());
                    pstm.setInt(4, value.getCapacidade());
                    pstm.setInt(5, value.getCadeira());
                    pstm.setInt(6, value.isPratica() ? 1 : 0);
                    pstm.setInt(7, value.isTeorica() ? 1 : 0);
                    pstm.setInt(8, key);
                    pstm.executeUpdate();
                }
            } else {
                // Insert new turno
                try (PreparedStatement pstm = conn.prepareStatement(
                        "INSERT INTO Turno (idTurno,Sala,Hora,DataT,Capacidade,Cadeira,Pratica,Teorica) " +
                                "VALUES (?,?,?,?,?,?,?,?)")) {
                    pstm.setInt(1, value.getIdTurno());
                    pstm.setString(2, value.getSala());
                    pstm.setString(3, value.getHora());
                    pstm.setString(4, value.getDataT());
                    pstm.setInt(5, value.getCapacidade());
                    pstm.setInt(6, value.getCadeira());
                    pstm.setInt(7, value.isPratica() ? 1 : 0);
                    pstm.setInt(8, value.isTeorica() ? 1 : 0);
                    pstm.executeUpdate();
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return value;
    }

    @Override
    public void putAll(Map<? extends Integer, ? extends Turno> m) {
        m.keySet().forEach(i -> this.put(i, m.get(i)));
    }

    @Override
    public Turno remove(Object key) {
        Turno value = null;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM Turno WHERE idTurno = ?")) {
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
             ResultSet rs = stm.executeQuery("SELECT count(*) FROM `HorarioSim`.`Turno`")) {
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
    public Collection<Turno> values() {
        return this.keySet().stream().map(this::get).collect(Collectors.toList());
    }
}


