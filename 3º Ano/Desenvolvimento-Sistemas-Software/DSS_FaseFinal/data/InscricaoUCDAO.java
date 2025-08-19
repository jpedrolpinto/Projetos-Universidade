package data;

import business.horario.Horario;
import business.uni.Inscricao;
import business.utilizadores.Aluno;

import java.awt.*;
import java.sql.*;
import java.util.*;
import java.util.List;
import java.util.stream.Collectors;

public class InscricaoUCDAO implements Map<Integer, Inscricao> {
    private static InscricaoUCDAO singleton = null;

    private InscricaoUCDAO() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            String sql = "CREATE TABLE IF NOT EXISTS `HorarioSim`.`Inscricao_UC` (" +
                    "Aluno INT NOT NULL," +
                    "UC INT NOT NULL," +
                    "FOREIGN KEY (Aluno) REFERENCES Aluno(id)," +
                    "FOREIGN KEY (UC) REFERENCES UC(idUC)," +
                    "PRIMARY KEY (Aluno, UC))";
            stm.executeUpdate(sql);

        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
    }

    public static InscricaoUCDAO getInstance() {
        if (InscricaoUCDAO.singleton == null) {
            InscricaoUCDAO.singleton = new InscricaoUCDAO();
        }
        return InscricaoUCDAO.singleton;
    }

    @Override
    public void clear() {
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement()) {
            stm.executeUpdate("TRUNCATE Inscricao_UC");
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
             ResultSet rs = stm.executeQuery("SELECT Aluno FROM `HorarioSim`.`Inscricao_UC` WHERE Aluno=" + key)) {
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
    public Set<Entry<Integer, Inscricao>> entrySet() {
        return this.keySet().stream().map(k -> Map.entry(k, this.get(k))).collect(Collectors.toSet());
    }

    @Override
    public Inscricao get(Object key) {
        Inscricao i = null;
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`Inscricao_UC` WHERE Aluno=" + key)) {
            i = rs.next() ? new Inscricao(rs.getInt("Aluno"), rs.getInt("UC")) : null;
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return i;
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
             ResultSet rs = stm.executeQuery("SELECT Aluno, UC FROM Inscricao_UC")) {
            while (rs.next()) {
                int aluno = rs.getInt("Aluno");
                int uc = rs.getInt("UC");
                int key = aluno * 1000 + uc; // Criando uma chave única combinando aluno e UC
                res.add(key);
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return res;
    }

    @Override
    public Inscricao put(Integer key, Inscricao value) {
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD)) {
            // Inserir diretamente usando os valores da inscrição
            try (PreparedStatement pstm = conn.prepareStatement(
                    "INSERT INTO Inscricao_UC (Aluno, UC) VALUES (?, ?)")) {
                pstm.setInt(1, value.getAluno());
                pstm.setInt(2, value.getUC());
                pstm.executeUpdate();
            }
        } catch (SQLException e) {
            throw new RuntimeException("Erro ao inserir inscrição: " + e.getMessage());
        }
        return value;
    }

    @Override
    public void putAll(Map<? extends Integer, ? extends Inscricao> m) {
        m.keySet().forEach(i -> this.put(i, m.get(i)));
    }

    @Override
    public Inscricao remove(Object key) {
        Inscricao value = null;
        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = conn.createStatement()) {
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM Inscricao_UC WHERE Aluno = ?")) {
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
             ResultSet rs = stm.executeQuery("SELECT count(*) FROM `HorarioSim`.`Inscricao_UC`")) {
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
    public Collection<Inscricao> values() {
        List<Inscricao> inscricoes = new ArrayList<>();
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM `HorarioSim`.`Inscricao_UC`")) {
            while (rs.next()) {
                inscricoes.add(new Inscricao(rs.getInt("Aluno"), rs.getInt("UC")));
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new NullPointerException(e.getMessage());
        }
        return inscricoes;
    }
}
