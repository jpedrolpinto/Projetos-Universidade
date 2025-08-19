{- |
Module      : Tarefa3_2022li1g055
Description : Movimentação do personagem e obstáculos
Copyright   : João Pedro Loureiro Pinto <a104270@alunos.uminho.pt>
              Diogo Alexandre Gomes Silva <a104183@alunos.uminho.pt>

Módulo para a realização da Tarefa 3 do projeto de LI1 em 2022/23.
-}
module Tarefa3_2022li1g055 where

import LI12223


{- |
== Funçao Principal:
A funçao animaJogo recebe um Jogo e uma Jogada e devolve o mesmo jogo que recebeu no incio so que com as alteraçoes proporcionadas pela Jogada,
fazendo isto utilizando as funçoes auxiliares. 
-}
animaJogo :: Jogo -> Jogada -> Jogo
animaJogo (Jogo j (Mapa l t)) jog = Jogo (moveJogador j (Mapa l t) jog) (Mapa l (map moveObstaculos t)) 


--1--5)
{- |
== Funçao Auxiliar:
A funçao moveObstaculos recebe um Terrebo e a sua respetiva lista de Obstaculos, no caso do Rio se a sua velocidade for negativa, ou seja se movimentar
para a esquerda, a funçao vai retirar da lista de obstaculos o resto da divisao do modulo da velocidade pelo comprimento da lista de obstaculos e vai 
juntar isso tudo ,por fim, no fim da lista de obstaculos, fazendo assim com que se moviemnte para a esquerda; No caso de a velocidade ser positiva, ou seja 
se movimentar para a direita, a funçao utilizará outras duas auxiliares, a lastN( esta da reverse á lista de obstaculo e pega nos primeiros n( sendo esta n a conta do resto 
da divisao do modulo da velocidade pelo comprimento da lista )) e a firstN( esta subtrai o compriemnto da lista de obstaculos ao n e dá take desse valor na lista de obstaculos),
logo fazendo a lastN primeiro e juntando no fim  a firstN da nos o movimento para a direita;
Sendo o mesmo processo para a Estrada. 
-}
moveObstaculos :: (Terreno, [Obstaculo]) -> (Terreno, [Obstaculo])
moveObstaculos (Rio v, o) | v < 0 = (Rio v, drop (mod (abs v) (length o)) o ++ take (mod (abs v) (length o)) o)
                          | v > 0 = (Rio v, lastN (mod (abs v) (length o)) o ++ firstN (mod (abs v) (length o)) o)
                          | otherwise = (Rio v, o)
moveObstaculos (Estrada v, o) | v < 0 = (Estrada v, drop (mod (abs v) (length o)) o ++ take (mod (abs v) (length o)) o)
                              | v > 0 = (Estrada v, lastN (mod (abs v) (length o)) o ++ firstN (mod (abs v) (length o)) o)
                              | otherwise = (Estrada v, o)
moveObstaculos t = t


lastN :: Int -> [a] -> [a]
lastN n l = take n (reverse l)

firstN :: Int -> [a] -> [a]
firstN n l = take ((length l) - n) l

--2--4)
{- |
== Funçao Auxiliar:
A funçao moveJogador recebe um Jogador, um Mapa e uma Jogada e devolvens um Jogador com as novas coordenadas depois de ser efetuada a jogada;
No caso do jogador se quiser movimentar para cima a funçao verifica se este se encontra no ponto de coordenadas onde y = 0, isto é onde acaba o mapa na parte de cima,
caso isto se verifique nao acontece nada e mantem se a posiçao do jogador, caso isto nao se verifique, a funçao utiliza outra auxiliar existeArvore( esta verifique se naquelas 
coordenadas existe uma arvore ), mais uma vez caso isto se verifique as coordenadas do jogador mantem-se, se nao o y das coordenadas aumenta 1, significando que o jogador se movimentou
para cima; No caso do jogador se quiser movimentar para baixo a funçao verifica se este se encontra no ponto de coordenadas onde y = (length t) - 1, isto é onde acaba o mapa na parte de baixo,
caso isto se verifique nao acontece nada e mantem se a posiçao do jogador, caso isto nao se verifique, a funçao utiliza outra auxiliar existeArvore, mais uma vez caso isto se verifique as 
coordenadas do jogador mantem-se, se nao o y das coordenadas diminui 1, significando que o jogador se movimentou para baixo;  Se o jogador se quiser movimentar para a esquerda a funçao verifica
se o x é igual a 0, caso isto se verifique  as coordenadas do jogador mantem se, caso isto nao se verifique é utilizada a funçao auxliar existeArvore, mais uma vez caso isto se verifique as coordenadas do jogador mantem-se, 
se nao o x das coordenadas diminui 1 significando que o jogador se movimentou para a esquerda; Se o jogador se quiser movimentar para a direita a funçao verifica se o x é igual a l - 1,
caso isto se verifique as coordenadas do jogador mantem se, caso isto nao se verifique é utilizada a funçao auxiliar existeArvore( esta verifique se naquelas coordenadas existe uma arvore ),
mais uma vez caso isto se verifique as coordenadas do jogador mantem-se, se nao o x das coordenadas aumenta 1 significando que o jogador se movimentou para a direita;
Por fim se nao existir jogada, ou seja, o jogador estiver parado as suas coordenadas mantem se, sofrendo estas alteraçao apenas se o jogador se encontrar em cima de um tronco
sendo somada a velocidade daquele rio ao x do jogador.
-}
moveJogador :: Jogador -> Mapa -> Jogada -> Jogador
moveJogador (Jogador (x,y)) (Mapa l t) (Move d) | d == Cima = if y == 0 then (Jogador (x,y))
                                                              else if existeArvore t (x, y - 1) then (Jogador (x,y))
                                                              else (Jogador (x, y - 1))
                                                | d == Baixo = if y == (length t) - 1 then (Jogador (x,y))
                                                               else if existeArvore t (x,y + 1) then (Jogador (x,y))
                                                               else (Jogador (x, y + 1))
                                                | d == Esquerda = if x == 0 then (Jogador (x,y))
                                                                  else if existeArvore t (x - 1, y) then (Jogador (x,y))
                                                                  else (Jogador (x - 1 + (velocidadeTronco t (x,y)), y))
                                                | d == Direita = if x == l - 1 then (Jogador (x,y))
                                                                  else if existeArvore t (x + 1, y) then (Jogador (x,y))
                                                                  else (Jogador (x + 1 + (velocidadeTronco t (x,y)), y))
--3)
moveJogador (Jogador (x,y)) (Mapa l t) (Parado) = (Jogador (x + (velocidadeTronco t (x,y)), y))


--Auxiliares)
{-|
A funçao existeArvore recebe uma lista de terrenos e os seus respetivos obstaculos e umas coordenadas, depois procura que terreno e lista de obstaculos está na coordenada y,
depois na lista de obstaculos procura o obstaculo que se encontra na coordenada x caso este seja Arvore a funçao dá True
-}
existeArvore :: [(Terreno, [Obstaculo])] -> Coordenadas -> Bool
existeArvore t (x,y) = (snd (t!!y))!!x == Arvore

{-|
A funçao velocidadeTronco recebe uma lista de terrenos e os seus respetivos obstaculos e umas coordenadas, depois procura que terreno e lista de obstaculos está na coordenada y,
depois na lista de obstaculos procura o obstaculo que se encontra na coordenada x caso este seja Tronco a funçao dá-nos o valor da velocidade aplicando a funçao auxiliar velocidade 
ao terreno que se encontra na coordenada y, isto é, se for relva será 0 se for Rio ou Estrada será v. 
-}
velocidadeTronco :: [(Terreno, [Obstaculo])] -> Coordenadas -> Int
velocidadeTronco t (x,y) | (snd (t!!y))!!x == Tronco = velocidade (fst (t!!y))
                         | otherwise = 0


velocidade :: Terreno -> Int
velocidade (Relva) = 0
velocidade (Rio v) = v
velocidade (Estrada v) = v