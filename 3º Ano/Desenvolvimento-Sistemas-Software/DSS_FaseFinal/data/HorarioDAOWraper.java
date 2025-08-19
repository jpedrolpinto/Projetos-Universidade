package data;

import business.horario.Horario;

import java.sql.*;
import java.util.*;
import java.util.stream.Collectors;

public class HorarioDAOWraper implements Map<Integer, List<Horario>> {
    private final HorarioDAO horarioDAO;

    private HorarioDAOWraper() {
        this.horarioDAO = HorarioDAO.getInstance();
    }

    public static HorarioDAOWraper getInstance() {
        return new HorarioDAOWraper();
    }

    @Override
    public int size() {
        return (int) horarioDAO.keySet().stream()
                .map(this::get)
                .filter(Objects::nonNull)
                .count();
    }

    @Override
    public boolean isEmpty() {
        return horarioDAO.isEmpty();
    }

    @Override
    public boolean containsKey(Object key) {
        return horarioDAO.containsKey(key);
    }

    @Override
    public boolean containsValue(Object value) {
        if (!(value instanceof List)) return false;
        List<?> list = (List<?>) value;
        return list.stream()
                .allMatch(item -> item instanceof Horario && horarioDAO.containsValue(item));
    }

    @Override
    public List<Horario> get(Object key) {
        if (!(key instanceof Integer)) return null;

        List<Horario> horarios = new ArrayList<>();
        try (Connection con = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD);
             Statement stm = con.createStatement();
             ResultSet rs = stm.executeQuery("SELECT * FROM Horario WHERE Aluno=" + key)) {
            while (rs.next()) {
                horarios.add(new Horario(rs.getInt("Aluno"),
                        rs.getInt("Regente"),
                        rs.getInt("Turno")));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return horarios;
    }

    @Override
    public List<Horario> put(Integer key, List<Horario> value) {
        List<Horario> previous = get(key);

        try (Connection conn = DriverManager.getConnection(DAOconfig.URL, DAOconfig.USERNAME, DAOconfig.PASSWORD)) {
            // Remove existing entries
            try (PreparedStatement pstm = conn.prepareStatement("DELETE FROM Horario WHERE Aluno = ?")) {
                pstm.setInt(1, key);
                pstm.executeUpdate();
            }

            // Insert new entries
            if (value != null && !value.isEmpty()) {
                try (PreparedStatement pstm = conn.prepareStatement(
                        "INSERT INTO Horario (Aluno, Regente, Turno) VALUES (?, ?, ?)")) {
                    for (Horario h : value) {
                        try {
                            pstm.setInt(1, h.getAluno());
                            pstm.setInt(2, h.getRegente());
                            pstm.setInt(3, h.getTurno());
                            pstm.executeUpdate();
                        } catch (SQLException e) {
                            // Log error but continue with other entries
                            System.err.println("Erro ao inserir horário: " + e.getMessage());
                        }
                    }
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
            throw new RuntimeException("Erro ao atualizar horários", e);
        }

        return previous;
    }

    @Override
    public List<Horario> remove(Object key) {
        List<Horario> previous = get(key);
        horarioDAO.remove(key);
        return previous;
    }

    @Override
    public void putAll(Map<? extends Integer, ? extends List<Horario>> m) {
        m.forEach(this::put);
    }

    @Override
    public void clear() {
        horarioDAO.clear();
    }

    @Override
    public Set<Integer> keySet() {
        return horarioDAO.keySet();
    }

    @Override
    public Collection<List<Horario>> values() {
        return horarioDAO.values().stream()
                .map(Collections::singletonList)
                .collect(Collectors.toList());
    }

    @Override
    public Set<Entry<Integer, List<Horario>>> entrySet() {
        return horarioDAO.entrySet().stream()
                .map(entry -> new AbstractMap.SimpleEntry<>(
                        entry.getKey(),
                        Collections.singletonList(entry.getValue())))
                .collect(Collectors.toSet());
    }
}
