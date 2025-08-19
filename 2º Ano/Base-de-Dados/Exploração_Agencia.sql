--              VIEWS & QUERIES


-- NOME E DATA DE NASCIMENTO DE TODOS OS CLIENTES
SELECT nome, data_nascimento FROM cliente;

-- NOME E DATA DE NASCIMENTO DE TODOS OS CLIENTES CUJO NOME COMEÇA COM REI
SELECT nome, data_nascimento FROM cliente WHERE nome LIKE 'Rei%';

-- DETETIVES DISPONIVEIS ( verificar quais detetives disponiveis ) 
SELECT n_identificação FROM detetive WHERE disponibilidade = 'disponivel';

-- NUMEROS DOS CASOS EXISTENTES
SELECT numero FROM caso ;



-- INVESTIGAÇÕES QUE ACABARAM NUM CERTO INTERVALO DE DATAS( ULTIMO ANO ) ( comparar casos com outros recentes ) 

CREATE VIEW InvestigacoesConcluidasUltimoAno AS
	SELECT caso.numero AS caso, caso.fk_nDetetive_Caso AS detetive
		FROM caso 
			WHERE caso.dta_fim BETWEEN DATE_SUB(CURDATE(), INTERVAL 1 YEAR) AND CURDATE();
            
SELECT * FROM InvestigacoesConcluidasUltimoAno;
-- VER A VISTA
  
  
-- OS 10 DETETIVES COM MAIS CASOS RESOLVIDOS ( se forem precisos os melhores detetives ) 
CREATE VIEW DetetivesComMaisCasosResolvidos AS
	SELECT detetive.n_identificação AS id, COUNT(caso.fk_nDetetive_Caso) AS casosResolvidos
		FROM caso JOIN detetive ON caso.fk_nDetetive_Caso = detetive.n_identificação
			WHERE caso.dta_fim IS NOT NULL
			GROUP BY detetive.n_identificação
			ORDER BY casosResolvidos DESC
            
		LIMIT 10;

SELECT * FROM DetetivesComMaisCasosResolvidos;
-- VER A VISTA


-- RECURSO COM MAIS STOCK ( verificar qual recurso tem mais stock e nao precisa de ser abastecido ) 
CREATE VIEW RecursoComMaisStock AS
	SELECT recursos.nome AS nome, recursos.stock AS stock
		FROM recursos
			ORDER BY stock DESC
	LIMIT 1;
            
SELECT * FROM RecursoComMaisStock;
-- VER A VISTA
DROP VIEW RecursoComMaisStock;



--              PROCEDURES & TRIGGERS & FUNCTIONS


-- TRIGGER PARA VERIFICAR SE O NUM JÁ EXISTE NA TABELA ( tornar automatico a verificação ) 
DELIMITER $$
CREATE TRIGGER before_insert_check_numero_caso
	BEFORE INSERT ON caso
		FOR EACH ROW
			BEGIN
				IF EXISTS (SELECT 1 FROM caso WHERE numero = NEW.numero) THEN
					SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'O número do caso já existe na tabela.';
				END IF;
			END$$
DELIMITER ;


-- PROCEDIMENTO PARA ADICIONAR NOVOS CASOS  ( gerenciar e otimizar a operação ) 
DELIMITER $$
CREATE PROCEDURE insere_caso(
    IN p_numero INT,
    IN p_dta_inicio DATE,
    IN p_dta_fim DATE,
    IN p_provas VARCHAR(50),
    IN p_fk_idTipo INT,
    IN p_fk_idCliente INT,
    IN p_fk_nDetetive_Caso INT
)
BEGIN
    -- Variável para armazenar a mensagem de erro do trigger
    DECLARE erro_msg VARCHAR(255) DEFAULT NULL;

    -- Tentar inserir novo caso
    BEGIN
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 erro_msg = MESSAGE_TEXT;
        END;

        INSERT INTO caso (numero, dta_inicio, dta_fim, provas, fk_idTipo, fk_idCliente, fk_nDetetive_Caso)
        VALUES (p_numero, p_dta_inicio, p_dta_fim, p_provas, p_fk_idTipo, p_fk_idCliente, p_fk_nDetetive_Caso);
    END;

    -- Se a mensagem de erro do trigger estiver definida, sinalizar o erro
    IF erro_msg IS NOT NULL THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = erro_msg;
    END IF;
END$$
DELIMITER ;

CALL insere_caso();
DROP PROCEDURE insere_caso;


-- PROCEDIMENTO PARA REMOVER CASOS
DELIMITER $$
CREATE PROCEDURE remove_caso( IN p_numero INT )
BEGIN
    -- Variável para armazenar a mensagem de erro do trigger
    DECLARE erro_msg VARCHAR(255) DEFAULT NULL;

    -- Verificar se o número do caso existe na tabela
    IF NOT EXISTS (SELECT 1 FROM Caso WHERE numero = p_numero) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'O número do caso não existe na tabela.';
    ELSE
        -- Tentar remover o caso
        BEGIN
            DECLARE EXIT HANDLER FOR SQLEXCEPTION
            BEGIN
                GET DIAGNOSTICS CONDITION 1 erro_msg = MESSAGE_TEXT;
            END;

            -- Iniciar transação
            START TRANSACTION;

            -- Remover o caso
            DELETE FROM Caso WHERE numero = p_numero;

            -- Commit da transação
            COMMIT;
        END;

        -- Se a mensagem de erro do trigger estiver definida, sinalizar o erro
        IF erro_msg IS NOT NULL THEN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = erro_msg;
        END IF;
    END IF;
END$$
DELIMITER ;

DROP PROCEDURE remove_caso;

CALL insere_caso('02', '1999-01-01', '1999-12-31', 'Descrição do assassino', '01', '05', '007');
CALL remove_caso(08);
-- VERIFICAR SE ESTÁ A FUNCIONAR



-- FUNÇÃO PARA VER QUANTAS INVESTIGAÇÕES UM DADO DETETIVE CONCLUIU
DELIMITER $$
CREATE FUNCTION NumeroInvestigacoesConcluidas( id_detetive INT ) RETURNS INT
READS SQL DATA
	BEGIN
		DECLARE num_investigacoes INT;

		-- Atribuir a contagem à variável
		SELECT COUNT(*) INTO num_investigacoes 
		FROM caso
		WHERE fk_nDetetive_Caso = id_detetive AND dta_fim IS NOT NULL;

		-- Retornar o número de investigações
		RETURN num_investigacoes;
	END$$
DELIMITER ;

SELECT NumeroInvestigacoesConcluidas(007);
-- VER OS RESULTADOS



--             INDICES

CREATE INDEX idx_fk_nDetetive_Caso ON caso(fk_nDetetive_Caso);

CREATE INDEX idx_dta_fim ON caso(dta_fim);


--          UTILIZADORES 

-- USER DIRETOR PINGUIM
CREATE USER 'pinguim'@'localhost' IDENTIFIED BY 'pinguim_pass';
GRANT ALL PRIVILEGES ON * TO 'pinguim'@'localhost' WITH GRANT OPTION;
FLUSH PRIVILEGES;

SHOW GRANTS FOR 'pinguim'@'localhost';


-- USER DETETIVE 007
CREATE USER '007'@'localhost' IDENTIFIED BY '007_pass';
GRANT SELECT, INSERT, UPDATE ON casos.* TO '007'@'localhost';
GRANT SELECT ON cliente.* TO '007'@'localhost';
GRANT SELECT ON tipo.* TO '007'@'localhost';
GRANT SELECT ON recursos.* TO '007'@'localhost';
FLUSH PRIVILEGES;

SHOW GRANTS FOR '007'@'localhost';


-- USER REI CARLOS
CREATE VIEW view_Rei_Carlos AS
SELECT * FROM caso
WHERE fk_idCliente = 01 ;

CREATE USER 'ReiCarlos'@'localhost' IDENTIFIED BY 'ReiCarlos_pass';
GRANT SELECT ON view_Rei_Carlos TO 'ReiCarlos'@'localhost';

SHOW GRANTS FOR 'ReiCarlos'@'localhost';
SELECT * FROM view_Rei_Carlos;























































-- QUERIES

-- filtragem e limitação ( tres detetives disponiveis )
SELECT * FROM detetive WHERE disponibilidade = 'disponivel' LIMIT 3;

-- interrogação, consulta e junção ( informaçao dos casos e a descrição do seu tipo ) 
SELECT c.numero, c.dta_inicio, c.dta_fim, t.descricao
FROM caso c JOIN tipo t ON c.fk_idTipo = t.id_Tipo;

-- filtragem e ordenação ( clientes com estuto Rei do masi velho para mais novo )
SELECT * FROM cliente WHERE estatuto = 'Rei'
ORDER BY data_nascimento ASC;

-- subquery ( Ajudantes que tem um caso do tipo Homicidio)
SELECT a.nome FROM ajudante a
	WHERE a.id_ajudante IN (
		SELECT aj.Ajudante_id_ajudante FROM ajuda aj
		JOIN caso c ON aj.Caso_numero = c.numero
		WHERE c.fk_idTipo = '01'
	);
    
-- agregação ( numero de casos de cada tipo )
SELECT t.descricao AS tipo, COUNT(c.numero) AS total FROM caso c
JOIN tipo t ON c.fk_idTipo = t.id_Tipo
GROUP BY t.descricao;


