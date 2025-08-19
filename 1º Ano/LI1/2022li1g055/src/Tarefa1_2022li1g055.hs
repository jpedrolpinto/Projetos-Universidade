{- |
Module      : Tarefa1_2022li1g055
Description : Validação de um mapa
Copyright   : João Pedro Loureiro Pinto <a104270@alunos.uminho.pt>
              Diogo Alexandre Gomes Silva <a104183@alunos.uminho.pt>

Módulo para a realização da Tarefa 1 do projeto de LI1 em 2022/23.
-}
module Tarefa1_2022li1g055 where

import LI12223


{- | 
== Função Principal:

A funçao mapaValido combina todas as funções auxiliares para verificar se o dado mapa é válido 
ou não
-}

mapaValido :: Mapa -> Bool
mapaValido (Mapa l t) = (obstaculosValidos t) && (riosContiguos t) 
    && (maxTronco t) && (maxCarro t) && (espacoLivre t)
    && (comprimento (Mapa l t)) && (maxTerrenos (Mapa l t))


{- | 
== Função Auxiliar:
    Estas funçoes verificam para cada terreno se os obstaculos que estao lá são válidos
Cada uma das funçoes recebe um Terreno e uma lista de Obstaculos e verifca quais os obstaculos naquele terreno,
sendo verdade caso respeite os parametros de cada uma.
-}

--1)
obstaculosValidos :: [(Terreno, [Obstaculo])] -> Bool
obstaculosValidos [] = True
obstaculosValidos ((Relva, o):t) = relvaValida (Relva, o)
obstaculosValidos ((Estrada v, o):t) = estradaValida (Estrada v, o)
obstaculosValidos ((Rio v, o):t) = rioValido (Rio v, o)


relvaValida :: (Terreno, [Obstaculo]) -> Bool
relvaValida (t, []) = True
relvaValida (Relva, (h:t)) | h == Nenhum || h == Arvore = relvaValida (Relva, t) 
                           | otherwise = False

estradaValida :: (Terreno, [Obstaculo]) -> Bool
estradaValida (t, []) = True
estradaValida (Estrada v, (h:t)) | h == Nenhum || h == Carro = estradaValida (Estrada v, t) 
                                 | otherwise = False

rioValido :: (Terreno, [Obstaculo]) -> Bool
rioValido (t, []) = True
rioValido (Rio v, (h:t)) | h == Nenhum || h == Tronco = rioValido (Rio v, t)  
                         | otherwise = False 

 
{- | 
== Função Auxiliar:
    A funçao riosContiguos recebe uma listacom terrenos e os seus respetivos obstaculos, para o caso de serem
dois rios na cabeca da lista, a funçao multiplica as velocidades dos dois se der menor que 0 vai voltar a testar 
com o segundo elemnto e o resto da lista. Se nao acontecer o caso anterior a funçao avança a head da lista e comeca 
a testar os proximos elementos 
-}


--2)
riosContiguos :: [(Terreno, [Obstaculo])] -> Bool
riosContiguos [] = True
riosContiguos [x] = True
riosContiguos ((Rio v1, o1):(Rio v2, o2):t) | v1 * v2 < 0 = riosContiguos ((Rio v2, o2):t)
                                          | otherwise = False
riosContiguos (h1:h2:t) = riosContiguos (h2:t)


{- |
== Função Auxiliar:
    A função maxTronco recebe uma lista de terrenos e seus respetivos obstaculos, aplicando a funçao auxiliar
troncoValido a uma lista de obstaculos, esta por sua vez usa outra auxiliar chamada troncoAux que serve 
para contar quantos troncos tem numa lista de obstaculos, caso conte mais de 5 troncos isto fara com que a funçao
de False tudo que seja 5 ou menos dará verdade  
-}

--3)
maxTronco :: [(Terreno, [Obstaculo])] -> Bool
maxTronco [] = True
maxTronco ((h, o):t) | troncoValido o = maxTronco t
                     | otherwise = False

troncoValido :: [Obstaculo] -> Bool
troncoValido [] = False 
troncoValido x | troncoAux 1 1 x == 1 = True 
               | otherwise = False

troncoAux :: Int -> Int -> [Obstaculo] -> Int 
troncoAux m n [] = n 
troncoAux m n (x:xs)
             | m == 7 = n + 1
             | x == Tronco = if m<6 then troncoAux (m+1) n xs else 0
             | x == Nenhum = troncoAux 1 n xs
             | x == Arvore = troncoAux 1 n xs
             | x == Carro = troncoAux 1 n xs



{- |
== Função Auxiliar:
    A função maxCarro recebe uma lista de terrenos e seus respetivos obstaculos, aplicando a funçao auxiliar
carroValido a uma lista de obstaculos, esta por sua vez usa outra auxiliar chamada carroAux que serve 
para contar quantos carros tem numa lista de obstaculos, caso conte mais de 3 carros isto fara com que a funçao
de False tudo que seja 3 ou menos dará verdade 
-}

--4)
maxCarro :: [(Terreno, [Obstaculo])] -> Bool
maxCarro [] = True
maxCarro ((h, o):t) | carroValido o = maxCarro t
                    | otherwise = False

carroValido :: [Obstaculo] -> Bool
carroValido [] = False 
carroValido x | carroAux 1 1 x == 1 = True 
              | otherwise = False

carroAux :: Int -> Int -> [Obstaculo] -> Int 
carroAux m n [] = n 
carroAux m n (x:xs)
             | m == 5 = n + 1
             | x == Carro = if m<4 then carroAux (m+1) n xs else 0
             | x == Nenhum = carroAux 1 n xs
             | x == Arvore = carroAux 1 n xs
             | x == Tronco = carroAux 1 n xs


{- | 
== Função Auxiliar:
    A função espacoLivre recebe uma lista de terrenos e a sua respetiva lista de obstaculos, utilizando a funçao
auxiliar espacoLivreAux, esta verifica se na lista de obstaculos existe um obstaculo que seja nenhum, logo se
a funçao auxiliar se verificar verdadeira entao vai ser aplicado outra vez o mesmo processo para o resto do mapa
-}


--5)
espacoLivre :: [(Terreno, [Obstaculo])] -> Bool
espacoLivre [] = False
espacoLivre ((h, o):t) | espacoLivreAux o = True 
                       | otherwise = espacoLivre t

espacoLivreAux :: [Obstaculo] -> Bool
espacoLivreAux [] = False
espacoLivreAux (h:t) | h == Nenhum = True
                     | otherwise = espacoLivreAux t



{- | 
== Função Auxiliar:
    A funçao comprimento recebe um Mapa com os seus terrenos e os respetivos obstaculos de cada terreno,
a funçao pega na lista de obstaculos e verifica se o comprimento dessa lista é igual a largura do mapa caso seja igual
vai continuar a testar para o resto do Mapa
-}


--6)
comprimento :: Mapa -> Bool
comprimento (Mapa l []) = True
comprimento (Mapa l ((_, o):t)) = if length o == l then comprimento (Mapa l t) else False


{- | 
== Função Auxiliar:
    As tres funçoes numTerrenoContiguos recebem uma lista com terrenos e os seus respetivos obstaculos,
estas tres funcoes servem como auxiliares a principal maxTerrenos, pois estas tres cada uma independente 
serve para contar quantos rios, estradas, relvas existem na lista que foi dada a funçao.
Já só depois na principal applica se as tres auxiliares e caso as tres auxiliares respeitem as condiçoes 
impostas o Mapa será verdade de outra forma nao   
-}


--7)
maxTerrenos :: Mapa -> Bool
maxTerrenos (Mapa _ x) = (numRiosContiguos x 0) <= 4 && (numEstradasContiguas x 0) <= 5 && (numRelvasContiguas x 0) <= 5

numRiosContiguos :: [(Terreno, [Obstaculo])] -> Int -> Int
numRiosContiguos [] n = n
numRiosContiguos ((Rio v, o):t) n = numRiosContiguos t (n+1)
numRiosContiguos (h:t) n = numRiosContiguos t 0

numEstradasContiguas :: [(Terreno, [Obstaculo])] -> Int -> Int
numEstradasContiguas [] n = n
numEstradasContiguas ((Estrada v, o):t) n = numEstradasContiguas t (n+1)
numEstradasContiguas (h:t) n = numEstradasContiguas t 0

numRelvasContiguas :: [(Terreno, [Obstaculo])] -> Int -> Int
numRelvasContiguas [] n = n
numRelvasContiguas ((Relva, o):t) n = numRelvasContiguas t (n+1)
numRelvasContiguas (h:t) n = numRelvasContiguas t 0

