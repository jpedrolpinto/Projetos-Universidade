package business.utils;

import business.horario.Turno;
import data.UtilizadoresDAO;
import business.utilizadores.Utilizador;

import java.util.List;
import java.util.stream.Collectors;

public class utilities {

    // Helper method to check if two turnos have a time conflict
    public static boolean hasTimeConflict(Turno turno1, Turno turno2) {
        if (!turno1.getDataT().equals(turno2.getDataT())) {
            return false; // Different days, no conflict
        }

        // Parse hours from the format "HH/HH"
        String[] hours1 = turno1.getHora().split("/");
        String[] hours2 = turno2.getHora().split("/");

        int start1 = Integer.parseInt(hours1[0]);
        int end1 = Integer.parseInt(hours1[1]);
        int start2 = Integer.parseInt(hours2[0]);
        int end2 = Integer.parseInt(hours2[1]);

        // Check if the time periods overlap
        return (start1 < end2 && start2 < end1);
    }

    public static int gerarProximoId() {
        List<Integer> ids = UtilizadoresDAO.getInstance().values().stream()
                .filter(u -> !u.isRegente())
                .map(Utilizador::getNumeroID)
                .sorted()
                .collect(Collectors.toList());

        if (ids.isEmpty()) return 10000;

        int ultimoId = ids.get(ids.size() - 1);

        // Se o último ID é 10000, o próximo é 11000
        if (ultimoId == 10000) {
            return 11000;
        }
        // Se termina em 111, passa para próxima série de milhares
        else if (ultimoId % 1000 == 111) {
            return ((ultimoId / 1000) + 1) * 1000;
        }
        // Se termina em 11, adiciona 89 para ir para próxima centena
        else if (ultimoId % 100 == 11) {
            return ultimoId + 89;
        }
        // Se termina em 1, adiciona 9
        else if (ultimoId % 10 == 1) {
            return ultimoId + 9;
        }
        // Caso contrário, adiciona 1
        else {
            return ultimoId + 1;
        }
    }

}
