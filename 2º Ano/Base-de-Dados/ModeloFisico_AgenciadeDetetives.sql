-- Universidade do Minho
-- Bases de Dados 2024
-- Caso de Estudo: Agencia de Detetives

-- -----------------------------------------------------
-- Schema Agencia
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `Agencia` ;

-- -----------------------------------------------------
-- Schema Agencia
-- -----------------------------------------------------
CREATE DATABASE IF NOT EXISTS `Agencia` ;
USE `Agencia`;


-- -----------------------------------------------------
-- Table `Agencia`.`Diretor`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`Diretor` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`Diretor` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `nome` VARCHAR(75) NOT NULL,
  `email` VARCHAR(100) NOT NULL,
  `telefone` INT NOT NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Agencia`.`Detetive`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`Detetive` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`Detetive` (
  `n_identificação` INT NOT NULL AUTO_INCREMENT,
  `data_nascimento` DATE NOT NULL,
  `disponibilidade` VARCHAR(50) NOT NULL,
  `telefone` INT NOT NULL,
  `fk_idDiretor` INT NOT NULL,
  PRIMARY KEY (`n_identificação`, `fk_idDiretor`),
  CONSTRAINT `fk_idDiretor`
    FOREIGN KEY (`fk_idDiretor`) REFERENCES `Agencia`.`Diretor` (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Agencia`.`Ajudante`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`Ajudante` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`Ajudante` (
  `id_ajudante` INT NOT NULL AUTO_INCREMENT,
  `nome` VARCHAR(75) NOT NULL,
  `data_nascimento` DATE NOT NULL,
  `especializacao` VARCHAR(100) NOT NULL,
  `fk_nDetetive` INT NOT NULL,
  PRIMARY KEY (`id_ajudante`, `fk_nDetetive`),
  CONSTRAINT `fk_nDetetive`
    FOREIGN KEY (`fk_nDetetive`)
    REFERENCES `Agencia`.`Detetive` (`n_identificação`)
)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Agencia`.`Tipo`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`Tipo` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`Tipo` (
  `id_Tipo` INT NOT NULL AUTO_INCREMENT,
  `descricao` TEXT NOT NULL,
  PRIMARY KEY (`id_Tipo`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Agencia`.`Cliente`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`Cliente` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`Cliente` (
  `id_Cliente` INT NOT NULL AUTO_INCREMENT,
  `nome` VARCHAR(75) NOT NULL,
  `data_nascimento` DATE NOT NULL,
  `estatuto` VARCHAR(50) NOT NULL,
  `telefone` INT NOT NULL,
  `fk_idDiretor_Cliente` INT NOT NULL,
  PRIMARY KEY (`id_Cliente`, `fk_idDiretor_Cliente`),
  CONSTRAINT `fk_idDiretor_Cliente`
    FOREIGN KEY (`fk_idDiretor_Cliente`)
    REFERENCES `Agencia`.`Diretor` (`id`)
)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Agencia`.`Caso`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`Caso` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`Caso` (
  `numero` INT NOT NULL AUTO_INCREMENT,
  `dta_inicio` DATE NOT NULL,
  `dta_fim` DATE NULL,
  `provas` VARCHAR(50) NOT NULL,
  `fk_idTipo` INT NOT NULL,
  `fk_idCliente` INT NOT NULL,
  `fk_nDetetive_Caso` INT NOT NULL,
  PRIMARY KEY (`numero`, `fk_idTipo`, `fk_nDetetive_Caso`, `fk_idCliente`),
  CONSTRAINT `fk_idTipo`
    FOREIGN KEY (`fk_idTipo`)
    REFERENCES `Agencia`.`Tipo` (`id_Tipo`),
  CONSTRAINT `fk_idCliente`
    FOREIGN KEY (`fk_idCliente`)
    REFERENCES `Agencia`.`Cliente` (`id_Cliente`),
  CONSTRAINT `fk_nDetetive_Caso`
    FOREIGN KEY (`fk_nDetetive_Caso`)
    REFERENCES `Agencia`.`Detetive` (`n_identificação`)
 )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Agencia`.`Classe`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`Classe` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`Classe` (
  `id_classe` INT NOT NULL AUTO_INCREMENT,
  `descricao` TEXT NOT NULL,
  PRIMARY KEY (`id_classe`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Agencia`.`Recursos`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`Recursos` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`Recursos` (
  `cod_Recurso` INT NOT NULL AUTO_INCREMENT,
  `stock` INT NOT NULL,
  `nome` VARCHAR(50) NOT NULL,
  `fk_idClasse` INT NOT NULL,
  `fk_nDetetive_Recursos` INT NOT NULL,
  PRIMARY KEY (`cod_Recurso`, `fk_idClasse`, `fk_nDetetive_Recursos`),
  CONSTRAINT `fk_nDetetive_Recursos`
    FOREIGN KEY (`fk_nDetetive_Recursos`)
    REFERENCES `Agencia`.`Detetive` (`n_identificação`),
  CONSTRAINT `fk_idClasse`
    FOREIGN KEY (`fk_idClasse`)
    REFERENCES `Agencia`.`Classe` (`id_classe`)
)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `Agencia`.`ajuda`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `Agencia`.`ajuda` ;

CREATE TABLE IF NOT EXISTS `Agencia`.`ajuda` (
  `Ajudante_id_ajudante` INT NOT NULL,
  `Caso_numero` INT NOT NULL,
  PRIMARY KEY (`Caso_numero`, `Ajudante_id_ajudante`),
  CONSTRAINT `fk_Ajudante_has_Caso_Ajudante1`
    FOREIGN KEY (`Ajudante_id_ajudante`)
    REFERENCES `Agencia`.`Ajudante` (`id_ajudante`),
  CONSTRAINT `fk_Ajudante_has_Caso_Caso1`
    FOREIGN KEY (`Caso_numero`)
    REFERENCES `Agencia`.`Caso` (`numero`)
)
ENGINE = InnoDB;


