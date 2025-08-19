package data;

import java.sql.*;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

import business.horario.Turno;
import business.horario.TurnoPreferencia;

public class TurnoPreferenciaDAO implements Map<Integer, TurnoPreferencia> {
    private static TurnoPreferenciaDAO singleton = null;

    private TurnoPreferenciaDAO() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            String sql = "CREATE TABLE IF NOT EXISTS `HorarioSim`.`Turno_Preferencia` ("+
                    "idTurno INT NOT NULL,"+
                    "Regente INT NOT NULL,"+
                    "TipoPreferencia VARCHAR(20) NOT NULL,"+
                    "ValorPreferencia VARCHAR(45) NOT NULL,"+
                    "FOREIGN KEY (idTurno) REFERENCES Turno(idTurno),"+
                    "FOREIGN KEY (Regente) REFERENCES Regente_Curso(id_Regente),"+
                    "PRIMARY KEY (idTurno, Regente))";
            stm.executeUpdate(sql);
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    public static TurnoPreferenciaDAO getInstance() {
        if (TurnoPreferenciaDAO.singleton == null) {
            TurnoPreferenciaDAO.singleton = new TurnoPreferenciaDAO();
        }
        return TurnoPreferenciaDAO.singleton;
    }

    @Override
    public void clear() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            stm.executeUpdate("TRUNCATE Turno_Preferencia");
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    @Override
    public boolean containsKey(Object key) {
        boolean r;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             PreparedStatement pstm = con.prepareStatement("SELECT idTurno FROM `HorarioSim`.`Turno_Preferencia` WHERE idTurno = ?")) {
            pstm.setInt(1, (Integer)key);
            try (ResultSet rs = pstm.executeQuery()) {
                r = rs.next();
            }
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
    public Set<Entry<Integer, TurnoPreferencia>> entrySet() {
        return this.keySet().stream().map(k -> Map.entry(k, this.get(k))).collect(Collectors.toSet());
    }

    @Override
    public TurnoPreferencia get(Object key) {
        TurnoPreferencia t = null;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`Turno_Preferencia` WHERE idTurno=" + key)) {
            t = rs.next() ? new TurnoPreferencia(rs.getInt("idTurno"), rs.getInt("Regente"), rs.getString("TipoPreferencia"), rs.getString("ValorPreferencia")) : null;
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
             ResultSet rs = stm.executeQuery("SELECT idTurno FROM Turno_Preferencia")) { // Era "FROM Aluno"
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
    public TurnoPreferencia put(Integer key, TurnoPreferencia value) {
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm2 = conn.prepareStatement("INSERT IGNORE INTO Turno_Preferencia (idTurno, Regente, TipoPreferencia, ValorPreferencia) VALUES (?, ?, ?, ?)")) {
                pstm2.setInt(1, value.getIdTurno());
                pstm2.setInt(2, value.getIdRegente());
                pstm2.setString(3, value.getTipoPreferencia());
                pstm2.setString(4, value.getValorPreferencia()); // This was previously set to index 3 twice

                pstm2.executeUpdate();
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return value;
    }


    @Override
    public void putAll(Map<? extends Integer, ? extends TurnoPreferencia> m) {
        m.keySet().forEach(i -> this.put(i, m.get(i)));
    }

    @Override
    public TurnoPreferencia remove(Object key) {
        TurnoPreferencia value = null;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM Turno_Preferencia WHERE idTurno = ?")) {
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
             ResultSet rs = stm.executeQuery("SELECT count(*) FROM `HorarioSim`.`Turno_Preferencia`")) {
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
    public Collection<TurnoPreferencia> values() {
        return this.keySet().stream().map(this::get).collect(Collectors.toList());
    }
}


