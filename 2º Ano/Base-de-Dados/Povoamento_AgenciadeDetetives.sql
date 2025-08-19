-- Universidade do Minho
-- Bases de Dados 2024
-- Caso de Estudo: Agencia de Detetives

-- -----------------------------------------------------
-- Indicação da base de dados de trabalho
-- -----------------------------------------------------
USE Agencia;

-- -----------------------------------------------------
-- Povoamento da tabela `diretor`
-- -----------------------------------------------------
INSERT INTO diretor(id, nome, email, telefone) VALUES
('01', 'Pinguim', 'pinguim_director@gmail.com', '943231765');

-- -----------------------------------------------------
-- Povoamento da tabela `detetive`
-- -----------------------------------------------------
INSERT INTO detetive(n_identificação, data_nascimento, disponibilidade, telefone, fk_idDiretor) VALUES
('001', '1998-04-21', 'indisponivel', '941221667', '01'),
('002', '1991-02-11', 'indisponivel', '932521863', '01'),
('003', '1991-05-13', 'indisponivel', '961320161', '01'),
('004', '1996-10-08', 'disponivel', '931221365', '01'),
('005', '1999-11-27', 'indisponivel', '961021668', '01'),
('006', '2000-12-19', 'indisponivel', '941195669', '01'),
('007', '1997-01-10', 'indisponivel', '960221660', '01');



-- -----------------------------------------------------
-- Povoamento da tabela `ajudante`
-- -----------------------------------------------------
INSERT INTO ajudante(id_ajudante, nome, data_nascimento, especializacao, fk_nDetetive) VALUES
('111', 'Alberto', '1998-04-21', 'Armas', '001'),
('222', 'José', '1991-02-11', 'Carros', '002'),
('333', 'Manuel', '1991-05-13', 'Armas', '003'),
('444', 'António', '1996-10-08', 'Computadores', '004'),
('555', 'João', '1999-11-27', 'Carros', '005'),
('666', 'Carlos', '2000-12-19', 'Armas', '006'),
('777', 'Tijolinho', '1997-01-10', 'Computadores', '007');

-- -----------------------------------------------------
-- Povoamento da tabela cliente
-- -----------------------------------------------------
INSERT INTO cliente(id_Cliente, nome, data_nascimento, estatuto, telefone, fk_idDiretor_Cliente) VALUES
('01','Rei Carlos', '1972-04-21', 'Rei', '969632455', '01'),
('02','Rainha Isabel', '1970-11-29', 'Rainha', '939355534', '01'),
('03','Princesa Inês', '1999-12-30', 'Princesa', '931345889', '01'),
('04','Dom Afonso Henriques', '1998-08-03', 'Rei', '964408365', '01'),
('05','Dona Maria II', '1969-10-25', 'Rainha', '939863217', '01'),
('06','Dom Dinis', '1966-03-10', 'Rei', '936098125', '01'),
('07','Dom Sancho Pança', '2004-06-28', 'Principe', '960976325', '01');

-- -----------------------------------------------------
-- Povoamento da tabela `tipo`
-- -----------------------------------------------------
INSERT INTO tipo(id_Tipo, descricao) VALUES
('01', 'Homicidio, aconteceu a morte de uma pessoa e cre-se que esta foi morta por outra pessoa!'),
('02', 'Suicidio, aconteceu a morte de uma pessoa e cre-se que a mesma se matou!'),
('03', 'Rapto, individuo desaparecido, acredita-se que tenha sido levado'),
('04', 'Roubo, bens de um certo individuo desapareceram ou foram levados!'),
('05', 'Terrorismo, atentados diversos!');

-- -----------------------------------------------------
-- Povoamento da tabela `caso`
-- -----------------------------------------------------
INSERT INTO caso(numero, dta_inicio, dta_fim, provas, fk_idTipo, fk_idCliente, fk_nDetetive_Caso) VALUES
('01', '2004-02-22', '2004-04-28', 'Arma do crime', '01', '01', '001'),
('02', '1981-07-14', '1983-01-05', 'Forca', '02', '06', '003'),
('03', '2008-06-12', '2008-06-13', 'Marcas de pneus', '03', '03', '005'),
('04', '2010-01-30', '2011-01-07', 'Impressões digitais', '04', '04', '002'),
('05', '2023-09-25', '2023-12-24', 'Ameaças de Bomba', '05', '07', '006'),
('06', '1999-01-01', '1999-12-31', 'Descrição do assassino', '01', '05', '007');

-- -----------------------------------------------------
-- Povoamento da tabela `classe`
-- -----------------------------------------------------
INSERT INTO classe(id_classe, descricao) VALUES
('01', 'Veiculos, diferentes tipos de transporte para os detetives se deslocarem!'),
('02', 'Armas, diversos tipos de armamento para ajudar os detetives em combate!'),
('03', 'Monetários, vários tipos de moedas para serem usados em diferentes países, vem em maços de 1000 euros!'),
('04', 'Tecnologia, tecnologias de ultima ponta que ajudam os detetives em diversas situações!'),
('05', 'Roupa, diferentes ocasiões pedem diferentes tipos de roupa!');

-- -----------------------------------------------------
-- Povoamento da tabela `recursos`
-- -----------------------------------------------------
INSERT INTO recursos(cod_Recurso, stock, nome, fk_idClasse, fk_nDetetive_Recursos) VALUES
('01', '1', 'Carro', '01', '007'),
('02', '2', 'Ak-47, pistola', '02', '002'),
('03', '4', 'Rádios', '04', '005'),
('04', '5', 'Arsenal de armas', '02', '001'),
('05', '5', 'Hungaros', '03', '003'),
('06', '1', 'Fato de gala', '04', '004');

-- -----------------------------------------------------
-- Povoamento da tabela `ajuda`
-- -----------------------------------------------------
INSERT INTO ajuda(Ajudante_id_ajudante, Caso_numero) VALUES
('111', '01'),
('333', '02'),
('555', '03'),
('222', '04'),
('666', '05'),
('777', '06');