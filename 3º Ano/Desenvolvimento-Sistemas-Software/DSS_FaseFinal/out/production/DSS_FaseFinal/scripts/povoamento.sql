DROP DATABASE `HorarioSim`;
-- CREATE DATABASE `HorarioSim`;
-- USE `HorarioSim`;

-- DELETE FROM `HorarioSim`.`Turno`; 
SELECT * FROM `HorarioSim`.`Horario`;
SELECT * FROM Horario WHERE Aluno = 10000;

INSERT INTO `HorarioSim`.`Utilizador` (Numero_id, Pass, Email, isRegente) VALUES
(01, '1234', 'albertoRegente@gmail.com', 1),
(10000, '1234', 'joaopinto@gmail.com', 0),
(11000, '1234', 'diogosilva@gmail.com', 0),
(11100, '1234', 'olavocarreira@gmail.com', 0),
(11110, '1234', 'miguelbarrocas@gmail.com', 0),
(11111, '1234', 'beatriz@gmail.com', 0),
(12000, '1234', 'carlos@gmail.com', 0),
(12100, '1234', 'diana@gmail.com', 0),
(12110, '1234', 'eduardo@gmail.com', 0),
(12111, '1234', 'fernanda@gmail.com', 0),
(13000, '1234', 'gabriel@gmail.com', 0),
(13100, '1234', 'helena@gmail.com', 0),
(13110, '1234', 'igor@gmail.com', 0),
(13111, '1234', 'julia@gmail.com', 0);

INSERT INTO `HorarioSim`.`Regente_Curso` (id_Regente, NomeRegente) VALUES
(01, 'Alberto dos Santos');

INSERT INTO `HorarioSim`.`Aluno` (id, Nome, Estatuto, Ano_Incricao, Media_Curso) VALUES
(10000, 'Joao Pinto', '', 3, 10),
(11000, 'Diogo Silva', '', 3, 13),
(11100, 'Olavo Carreira', '', 3, 15),
(11110, 'Miguel Barrocas', 'Especial', 3, 11),
(11111, 'Beatriz Costa', '', 3, 12),
(12000, 'Carlos Antunes', '', 3, 14),
(12100, 'Diana Mendes', 'Especial', 3, 11),
(12110, 'Eduardo Ribeiro', '', 3, 13),
(12111, 'Fernanda Lima', '', 3, 12),
(13000, 'Gabriel Nunes', '', 3, 14),
(13100, 'Helena Rocha', 'Especial', 3, 15),
(13110, 'Igor Santos', '', 3, 13),
(13111, 'Julia Martins', '', 3, 10);

INSERT INTO `HorarioSim`.`Semestre` (Numero_Semestre, Ano_Letivo, Periodo) VALUES
(1, 1, '07/07/2024 a 12/01/2025'),
(2, 1, '13/01/2025 a 30/06/2025'),
(3, 2, '07/07/2025 a 12/01/2026'),
(4, 2, '13/01/2026 a 30/06/2026'),
(5, 3, '07/07/2026 a 12/01/2027'),
(6, 3, '13/01/2027 a 30/06/2027');

INSERT INTO `HorarioSim`.`Curso` (CodigoCurso, NomeCurso, NumeroSemestre, NumeroAlunos, Regente) VALUES
(1500, 'LEI', 2, 170, 01);

INSERT INTO `HorarioSim`.`UC` (idUC, NomeUC, Ano, Semestre, Curso) VALUES
(01, 'Calculo de Programas', 3, 5, 1500),
(02, 'Comunicações por Computador', 3, 5, 1500),
(03, 'DSS', 3, 5, 1500),
(04, 'Inteligencia Artifical', 3, 5, 1500),
(05, 'Laboratorios Informatica IV', 3, 5, 1500),
(06, 'Sistemas Distribuidos', 3, 5, 1500);

INSERT INTO `HorarioSim`.`Turno` (idTurno, Sala, Hora, DataT, Capacidade, Pratica, Teorica, Cadeira) VALUES
(01, '2.18', '9/11', 'Segunda', 25, 1, 0, 04),
(02, '2.09', '9/11', 'Segunda', 25, 1, 0, 03),
(03, '2.09', '11/13', 'Segunda', 25, 1, 0, 04),
(04, '1.27', '11/13', 'Segunda', 25, 1, 0, 04),
(05, '2.11', '11/13', 'Segunda', 25, 1, 0, 03),
(06, '0.11', '11/13', 'Segunda', 25, 1, 0, 03),
(07, '0.14', '14/16', 'Segunda', 25, 1, 0, 04),
(08, '1.03', '14/16', 'Segunda', 25, 1, 0, 03),
(09, '0.08', '16/18', 'Segunda', 60, 0, 1, 04),
(10, '2.07', '18/20', 'Segunda', 25, 1, 0, 04),
(11, '1.22', '9/11', 'Terça', 25, 1, 0, 06),
(12, '2.18', '9/11', 'Terça', 25, 1, 0, 02),
(13, '2.11', '11/13', 'Terça', 25, 1, 0, 04),
(14, '1.04', '11/13', 'Terça', 25, 1, 0, 06),
(15, '2.03', '11/13', 'Terça', 25, 1, 0, 03),
(16, '1.21', '11/13', 'Terça', 25, 1, 0, 02),
(17, '1.13', '11/13', 'Terça', 25, 1, 0, 02),
(18, '0.11', '14/15', 'Terça', 25, 1, 0, 05),
(19, '0.11', '15/16', 'Terça', 25, 1, 0, 05),
(20, '0.08', '16/18', 'Terça', 60, 0, 1, 06),
(21, '1.22', '18/20', 'Terça', 25, 1, 0, 01),
(22, '1.27', '9/11', 'Quarta', 25, 1, 0, 01),
(23, '2.12', '11/13', 'Quarta', 25, 1, 0, 02),
(24, '2.07', '11/13', 'Quarta', 25, 1, 0, 01),
(25, '2.10', '11/13', 'Quarta', 25, 1, 0, 01),
(26, '2.02', '14/15', 'Quarta', 25, 1, 0, 05),
(27, '2.02', '15/16', 'Quarta', 25, 1, 0, 05),
(28, '0.08', '16/18', 'Quarta', 60, 0, 1, 03),
(29, '0.08', '18/20', 'Quarta', 60, 0, 1, 05),
(30, '0.17', '09/11', 'Quinta', 25, 1, 0, 06),
(31, '2.01', '09/11', 'Quinta', 25, 1, 0, 06),
(32, '2.11', '09/11', 'Quinta', 25, 1, 0, 02),
(33, '2.01', '11/13', 'Quinta', 25, 1, 0, 06),
(34, '2.11', '11/13', 'Quinta', 25, 1, 0, 02),
(35, '0.07', '16/18', 'Quinta', 60, 0, 1, 02),
(36, '0.20', '16/18', 'Quinta', 60, 0, 1, 01),
(37, '0.07', '09/11', 'Sexta', 60, 0, 1, 02),
(38, '0.01', '09/11', 'Sexta', 60, 0, 1, 01),
(39, '1.12', '11/13', 'Sexta', 25, 1, 0, 06),
(40, '2.14', '11/13', 'Sexta', 25, 1, 0, 06),
(41, '2.01', '11/13', 'Sexta', 25, 1, 0, 02),
(42, '2.06', '14/16', 'Sexta', 25, 1, 0, 03);

INSERT INTO `HorarioSim`.`Inscricao_UC` (Aluno, UC) VALUES

-- Joao Pinto (10000)
(10000, 01), (10000, 02), (10000, 03), (10000, 04), (10000, 05), (10000, 06),

-- Diogo Silva (11000)
(11000, 01), (11000, 02), (11000, 03), (11000, 04), (11000, 05), (11000, 06),

-- Olavo Carreira (11100)
(11100, 01), (11100, 02), (11100, 03), (11100, 04), (11100, 05), (11100, 06),

-- Beatriz Costa (11111)
(11111, 01), (11111, 02), (11111, 03), (11111, 04), (11111, 05), (11111, 06),

-- Carlos Antunes (12000)
(12000, 01), (12000, 02), (12000, 03), (12000, 04), (12000, 05), (12000, 06),

-- Eduardo Ribeiro (12110)
(12110, 01), (12110, 02), (12110, 03), (12110, 04), (12110, 05), (12110, 06),

-- Fernanda Lima (12111)
(12111, 01), (12111, 02), (12111, 03), (12111, 04), (12111, 05), (12111, 06),

-- Gabriel Nunes (13000)
(13000, 01), (13000, 02), (13000, 03), (13000, 04), (13000, 05), (13000, 06),

-- Igor Santos (13110)
(13110, 01), (13110, 02), (13110, 03), (13110, 04), (13110, 05), (13110, 06),

-- Julia Martins (13111)
(13111, 01), (13111, 02), (13111, 03), (13111, 04), (13111, 05), (13111, 06),

-- Alunos com estatuto especial - inscrevem apenas em algumas UCs

-- Miguel Barrocas (11110) - Especial
(11110, 01), -- Calculo de Programas
(11110, 03), -- DSS
(11110, 05), -- Laboratorios Informatica IV
(11110, 06), -- Sistemas Distribuidos

-- Diana Mendes (12100) - Especial
(12100, 02), -- Comunicações por Computador
(12100, 04), -- Inteligencia Artifical
(12100, 06), -- Sistemas Distribuidos

-- Helena Rocha (13100) - Especial
(13100, 01), -- Calculo de Programas
(13100, 03), -- DSS
(13100, 05); -- Laboratorios Informatica IV

INSERT INTO `HorarioSim`.`Horario` (Aluno, Regente, Turno) VALUES
(10000, 01, 10),  -- IA (UC 04)
(11000, 01, 21),  -- CP (UC 01)
(11100, 01, 29),  -- LI4 (UC 05)
(11111, 01, 36),  -- CP (UC 01)
(12000, 01, 37),  -- CC (UC 02)
(12110, 01, 39),  -- SD (UC 06)
(12111, 01, 10),  -- IA (UC 04)
(13000, 01, 21),  -- CP (UC 01)
(13110, 01, 30),  -- SD (UC 06)
(13111, 01, 36),  -- CP (UC 01)

-- Alunos com estatuto especial - só podem ter turnos das UCs em que estão inscritos
-- Miguel Barrocas (11110) está inscrito em: CP (01), DSS (03), LI4 (05), SD (06)
(11110, 01, 30),  -- SD (UC 06)

-- Diana Mendes (12100) está inscrita em: CC (02), IA (04), SD (06)
(12100, 01, 38),  -- CP (UC 01) 
(12100, 01, 12),  -- CC (UC 02)

-- Helena Rocha (13100) está inscrita em: CP (01), DSS (03), LI4 (05)
(13100, 01, 29);  -- LI4 (UC 05)
