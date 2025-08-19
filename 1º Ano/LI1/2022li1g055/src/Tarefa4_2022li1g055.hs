{- |
Module      : Tarefa4_2022li1g055
Description : Determinar se o jogo terminou
Copyright   : João Pedro Loureiro Pinto <a104270@alunos.uminho.pt>
              Diogo Alexandre Gomes Silva <a104183@alunos.uminho.pt>

Módulo para a realização da Tarefa 4 do projeto de LI1 em 2022/23.
-}
module Tarefa4_2022li1g055 where

import LI12223


{- |
== Função Principal:
A funçao jogoTerminou aplica a função auxiliar jogoTerminouAux ás coordenadas em que o jogador se encontra e á largura do mapa; 
utiliza tambem a funçao auxiliar terrenoNasCoordenadas aplicada á lista de terrenos e obstaculos do mapa dado e as coordenadas do jogador. 
-}
jogoTerminou :: Jogo -> Bool
jogoTerminou (Jogo (Jogador (x, y)) (Mapa l t)) = jogoTerminouAux (x, y) l (terrenoNasCoordenadas t (x, y)) 


{- |
== Funçao Auxiliar:
A Funçao jogoTerminouAux recebe umas coordenadas do jogador, um inteiro que corresponderá a largura do mapa e ainda um terreno
e os seus respetivos obstaculos; quando o terreno for relva e a coordenada em x for menor que 0, maior ou igual a largura 
do mapa corresponde a True, ou seja que o jogo terminou; no caso do terreno ser Rio e a coordenada em x for menor que 0, maior ou igual a largura 
do mapa ou ainda a coordenada x corresponder a um obstaculo Nenhum, isto significa que o jogo terminou; por fim se o terreno for Estrada e mais uma vez 
a coordenada em x for menor que 0, maior ou igual a largura do mapa ou ainda a coordenada x corresponder a um obstaculo Carro, isto significa que o jogo terminou. 
-}
jogoTerminouAux :: Coordenadas -> Int -> (Terreno, [Obstaculo]) -> Bool
jogoTerminouAux (x, y) l (Relva, o) = x < 0 || x >= l
jogoTerminouAux (x, y) l (Rio v, o) = x < 0 || x >= l || x `elem` (elemIndices Nenhum o) 
jogoTerminouAux (x, y) l (Estrada v, o) = x < 0 || x >= l || x `elem` (elemIndices Carro o) 


{- |
== Funçao Auxiliar:
A funçao terrenoNasCoordenadas recebe uma lista de terrenos e seus respetivos obstaculos e umas coordenadas, esta funçao vai procurar que terreno se 
encontra nas coordenadas dadas e é isso que a funçao nos vai dar, isto é, o terreno que se encontra naquelas coordenadas. 
-}
terrenoNasCoordenadas :: [(Terreno, [Obstaculo])] -> Coordenadas -> (Terreno, [Obstaculo])
terrenoNasCoordenadas t (x, y) = t!!y


{- |
== Funçao Auxiliar:
A funçao elemIndices calcula a lista de posições em que um dado elemento ocorre numa lista.
-}
elemIndices :: Eq a => a -> [a] -> [Int]
elemIndices _ [] = []
elemIndices x (h:t)
    | x == h = 0 : map (+1) (elemIndices x t) -- funçao map aplica uma funçao a todos os elementos de uma lista.
    | otherwise = map (+1) (elemIndices x t)