{- |
Module      : Tarefa2_2022li1g055
Description : Geração contínua de um mapa
Copyright   : João Pedro Loureiro Pinto <a104270@alunos.uminho.pt>
              Diogo Alexandre Gomes Silva <a104183@alunos.uminho.pt>

Módulo para a realização da Tarefa 2 do projeto de LI1 em 2022/23.
-}
module Tarefa2_2022li1g055 where

import LI12223
import Tarefa1_2022li1g055

{- |
== Funçao Principal:
 A funçao estendeMapa receb um Mapa e um inteiro de 0 a 100, a funçao vai gerar uma nova linha do mapa
e dependendo do numero escolhido poderá gerar um rio, estrada ou relva, isto tudo dentro dos parametros definidos nas funçoes auxiliares 
-}

estendeMapa :: Mapa -> Int -> Mapa
estendeMapa (Mapa l ((t,o):xs)) n = Mapa l ((proxTerreno, obstaculos) : ((t,o):xs))
    where 
        proxTerreno = adicionaVelocidade (escolheTerreno (proximosTerrenosValidos (Mapa l ((t,o):xs))) n) t n
        obstaculos = constroiObstaculos l (proxTerreno, []) n


{- |
== Funçao Auxiliar:
    A funçao proximosTerrenosValidos recebe um mapa com os seus respetivos terrenos e obstaculos, esta funçao vai verificar os ultimos terrenos da
 lista ou seja a sua head e caso esteja dentro de um dos casos definidos na função como por exemplo se os ultimos terrenos forem 4 Rios 
 os proximos terrenos só podeem ser Estrada ou Relva no caso de serem 4 Estradas só pode ser Rio ou Relva, ou se os ultimos terrenos forem 4 Relvas 
 só podem ser um Rio ou Estrada, no caso de os ultimos terrenos ou a head da lista nao se enquadrar em nenhum dos parametros anteriores
 os proximos terrenos válidos poderão ser qualquer um.
-}
proximosTerrenosValidos :: Mapa -> [Terreno]
proximosTerrenosValidos (Mapa _ ((Rio _, _):(Rio _, _):(Rio _, _):(Rio _, _):t)) = [Estrada 0, Relva]
proximosTerrenosValidos (Mapa _ ((Estrada _, _):(Estrada _, _):(Estrada _, _):(Estrada _, _):(Estrada _, _):t)) = [Rio 0, Relva]
proximosTerrenosValidos (Mapa _ ((Relva, _):(Relva, _):(Relva, _):(Relva, _):(Relva, _):t)) = [Rio 0, Estrada 0]
proximosTerrenosValidos m = [Rio 0, Estrada 0, Relva]



{- |
== Funçao Auxiliar:
    A funçao proximosObstaculosValidos recebe um inteiro,  que representa a largura do mapa, e recebe tambem um terreno com a sua lista de obstaculos, esta
função depnde do terreno que será, no caso de o terreno ser um Rio a função verifica o comprimento da lista de obstaculos se esta for igual a largura do mapa entao
da lista vazia, ou seja, nao pode ser adicionado nenhum obstaculo, no caso do comprimento da lista de obstaculos ser menor que a largura a funçao utiliza a funçao auxiliar
espacoLivreAux, que verifica se existe algum obstaculo Nenhum na lista caso isto seja falso o unico obstaculo possivel de ser adicionado será Nenhum, se ja existir um obstaculo 
Nenhum a funçao usará outra funçao auxiliar troncoValido que verifica se a quantidade de troncos na funçao é válida quando se adiciona um Tronco a lista de obstaculos que a funçao
recebe caso isto seja verdade os obstaculos válidos serao Nenhum e Tronco.
Sendo este mesmo processo para os dois outros terrenos.
-}

proximosObstaculosValidos :: Int -> (Terreno, [Obstaculo]) -> [Obstaculo]
proximosObstaculosValidos l (Rio _, o) | length o == l = []
                                       | length o == l - 1 && espacoLivreAux o == False = [Nenhum]
                                       | troncoValido (o ++ [Tronco]) == True = [Tronco, Nenhum]
                                       | otherwise = [Nenhum]

proximosObstaculosValidos l (Estrada _, o) | length o == l = []
                                           | length o == l - 1 && espacoLivreAux o == False = [Nenhum]
                                           | carroValido (o ++ [Carro]) == True = [Nenhum, Carro]
                                           | otherwise = [Nenhum]

proximosObstaculosValidos l (Relva, o) | length o == l = []
                                       | length o == l - 1 && espacoLivreAux o == False = [Nenhum]
                                       | otherwise = [Nenhum, Arvore]




{- |
== Funçao Auxiliar:
 A funçao escolheTerreno recebe uma lista de terrenos e um inteiro, pega no comprimento da lista de terrenos tira um e multiplica pelo inteiro, divide isto tudo por 100 e depois arredonda 
 com o numero que dá procura essa posição na lista inicial de terrenos, dando o terreno que se encontra nessa posiçao
-}
escolheTerreno :: [Terreno] -> Int -> Terreno
escolheTerreno t n = t!!(round (fromIntegral((n * ((length t) - 1)) :: Int) / 100))



{- |
== Funçao Auxiliar:
 A funçao adicionaVelocidade recebe dois terrenos e um inteiro, caso os terrenos sejam dois rios e a velocidade do segundo for maior que 0 
 entao a funçao vai adicionar uma velocidade negativa ao primeiro rio isto de acordo com o inteiro que a funçao receber.
 Dependendo do n(Int) a velociade dos terrenos vai ser diferente 
-}
adicionaVelocidade :: Terreno -> Terreno -> Int -> Terreno
adicionaVelocidade (Rio _) (Rio v) n = if v > 0 then (Rio (-1 * ((mod n 5) + 1))) else (Rio ((mod n 5) + 1))
adicionaVelocidade (Rio _) _ n = if n < 50 then (Rio (-1 * ((mod n 5) + 1))) else (Rio ((mod n 5) + 1))
adicionaVelocidade (Estrada _) _ n = if n < 50 then (Estrada (-1 * ((mod n 5) + 1))) else (Estrada ((mod n 5) + 1))
adicionaVelocidade (Relva) _ n = Relva



{- |
== Funçao Auxiliar:
 A funçao escolheObstaculo recebe uma lista de obstaculos e um inteiro, pega no inteiro dado e no comprimento da lista de obstaculos 
 faz a sua divisao e devolve o seu resto sendo sempre 1 ou 0, depois procura na lista de obstaculo que corresponde a [Nenhum,Arvore/Tronco/Carro]
 se o resto for 0 devolve Nenhum, se for 1 devolve um daqueles tres obstaculos dependendo de qual o terreno.
-}
escolheObstaculo :: [Obstaculo] -> Int -> Obstaculo
escolheObstaculo t n = t!!(mod n (length t))


{- |
== Funçao Auxiliar:
 A funçao constroiObstaculos recebe um inteiro que corresponde a largura, recebe um terreno e a sua lista de obstaculos e ainda recebe outro inteiro,
 primeiro a funçao testa se a largura do Mapa é igual ao comprimento da lista de obstaculos, caso isto se verifique vai nos dar a mesma lista de obstaculos 
 não adicionando outro. Para o caso de isto nao se verificar vai ser usada a funçao proximosObstaculosValidos para verificar que obstaculos podem ser adicionados 
 depois utiliza a funçao escolheObstaculo para escolher um obstaculo, este obstaculo é adicionado á lista anterior de obstaculos repetindo este processo até 
 que a lista de obstaculos tenha o mesmo comprimento da largura do Mapa.
-}
constroiObstaculos :: Int -> (Terreno, [Obstaculo]) -> Int -> [Obstaculo]
constroiObstaculos l (t, o) n | l == length o = o
                              | otherwise = constroiObstaculos l (t, o ++ [(escolheObstaculo (proximosObstaculosValidos l (t, o)) n)]) n
