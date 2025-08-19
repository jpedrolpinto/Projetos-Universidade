show databases;
show tables;

USE HorarioSim;

-- -------------------------------------
-- CREATES THE "UTILIZADOR" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Utilizador` (
	Numero_id INT NOT NULL,
    Pass VARCHAR(10) NOT NULL,
    Email VARCHAR(45) NOT NULL,
    isRegente INT NOT NULL, -- 1 se true, 0 se false
    PRIMARY KEY (Numero_id)
);

-- -------------------------------------
-- CREATES THE "REGENTE" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Regente_Curso` (
	id_Regente INT NOT NULL,
    NomeRegente VARCHAR(45) NOT NULL,
    FOREIGN KEY (id_Regente) REFERENCES Utilizador(Numero_id),
    PRIMARY KEY (id_Regente)
);

-- -------------------------------------
-- CREATES THE "ALUNO" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Aluno` (
	id INT NOT NULL,
    Nome VARCHAR(45) NOT NULL,
    Estatuto VARCHAR(45) NOT NULL,
    Ano_Incricao INT NOT NULL,
    Media_Curso INT NOT NULL,
    FOREIGN KEY (id) REFERENCES Utilizador(Numero_id),
    PRIMARY KEY (id)
);

-- -------------------------------------
-- CREATES THE "CURSO" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Curso` (
	CodigoCurso INT NOT NULL,
    NomeCurso VARCHAR(45) NOT NULL,
    NumeroSemestre INT NOT NULL,
    NumeroAlunos INT NOT NULL,
    Regente INT NOT NULL,
    FOREIGN KEY (Regente) REFERENCES Regente_Curso(id_Regente),
    PRIMARY KEY (CodigoCurso)
);

-- -------------------------------------
-- CREATES THE "SEMESTRE" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Semestre` (
	Numero_Semestre INT NOT NULL,
    Ano_Letivo INT NOT NULL,
    Periodo VARCHAR(23) NOT NULL,
    PRIMARY KEY (Numero_Semestre)
);

-- -------------------------------------
-- CREATES THE "UC" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`UC` (
	idUC INT NOT NULL,
    NomeUC VARCHAR(45) NOT NULL,
    Ano INT NOT NULL,
    Semestre INT NOT NULL,
    Curso INT NOT NULL,
    FOREIGN KEY (Semestre) REFERENCES Semestre(Numero_Semestre),
    FOREIGN KEY (Curso) REFERENCES Curso(CodigoCurso),
    PRIMARY KEY (idUC)
);

-- -------------------------------------
-- CREATES THE "Turno" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Turno` (
	idTurno INT NOT NULL,
    Sala VARCHAR(45) NOT NULL,
    Hora VARCHAR(5) NOT NULL,
    DataT VARCHAR(10) NOT NULL,
    Capacidade INT NOT NULL,
    Cadeira INT NOT NULL,
    Pratica INT NOT NULL, -- 1 se true, 0 se false
    Teorica INT NOT NULL, -- 1 se true, 0 se false
    FOREIGN KEY (Cadeira) REFERENCES UC(idUC),
    PRIMARY KEY (idTurno)
);

-- -------------------------------------
-- CREATES THE "HORARIO" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Horario` (
    Aluno INT NOT NULL,
    Regente INT NOT NULL,
    Turno INT NOT NULL,
    FOREIGN KEY (Aluno) REFERENCES Aluno(id),
    FOREIGN KEY (Regente) REFERENCES Regente_Curso(id_Regente),
    FOREIGN KEY (Turno) REFERENCES Turno(idTurno),
    PRIMARY KEY (Aluno, Turno)  -- Chave primária composta
);

-- -------------------------------------
-- CREATES THE "Inscricao_UC" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Inscricao_UC` (
    Aluno INT NOT NULL,
    UC INT NOT NULL,
    FOREIGN KEY (Aluno) REFERENCES Aluno(id),
    FOREIGN KEY (UC) REFERENCES UC(idUC),
    PRIMARY KEY (Aluno, UC)  -- Chave primária composta
);

-- -------------------------------------
-- CREATES THE "TURNO_PREFERENCIA" TABLE
-- -------------------------------------
CREATE TABLE IF NOT EXISTS `HorarioSim`.`Turno_Preferencia` (
    idTurno INT NOT NULL,
    Regente INT NOT NULL,
    TipoPreferencia VARCHAR(20) NOT NULL, -- 'ESTATUTO_ESPECIAL' ou 'MEDIA_MINIMA'
    ValorPreferencia VARCHAR(45) NOT NULL, -- Valor do estatuto ou média
    FOREIGN KEY (idTurno) REFERENCES Turno(idTurno),
    FOREIGN KEY (Regente) REFERENCES Regente_Curso(id_Regente),
    PRIMARY KEY (idTurno, Regente)  -- Chave primária composta
);

 -- DROP TABLE `HorarioSim`.`Horario`;

