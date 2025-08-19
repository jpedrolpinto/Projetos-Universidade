module Tarefa1_2022li1g055_Spec where

import LI12223
import Tarefa1_2022li1g055
import Test.HUnit


test1 :: Test
test1 = TestLabel "Testes Tarefa 1" $ test ["Teste 1" ~: True ~=? mapaValido (Mapa 3 [(Rio 3, [Nenhum, Nenhum, Tronco]), (Relva, [Nenhum, Arvore, Nenhum]), (Estrada 2, [Nenhum, Carro, Carro])])]

test2 :: Test
test2 = TestLabel "Testes Tarefa 1" $ test ["Teste 2" ~: True ~=? mapaValido (Mapa 3 [(Rio 3, [Nenhum, Nenhum, Tronco]), (Rio (-8), [Nenhum, Tronco, Nenhum]), (Estrada 2, [Nenhum, Carro, Carro])])]

test3 :: Test
test3 = TestLabel "Testes Tarefa 1" $ test ["Teste 3" ~: True ~=? mapaValido (Mapa 3 [(Rio 3, [Nenhum, Nenhum, Tronco]), (Estrada 2, [Nenhum, Carro, Carro]), (Rio 8, [Nenhum, Tronco, Nenhum])])]

test4 :: Test
test4 = TestLabel "Testes Tarefa 1" $ test ["Teste 4" ~: True ~=? mapaValido (Mapa 1 [(Rio 3, [Nenhum]), (Relva, [Nenhum]), (Estrada 2, [Nenhum])])]

test5 :: Test
test5 = TestLabel "Testes Tarefa 1" $ test ["Teste 5" ~: True ~=? mapaValido (Mapa 1 [(Rio 3, [Nenhum]), (Relva, [Nenhum]), (Estrada 2, [Nenhum]), (Rio 8, [Nenhum]), (Relva, [Nenhum]), (Estrada 1, [Nenhum]), (Rio 9, [Nenhum]), (Relva, [Nenhum]), (Estrada 7, [Nenhum]), (Rio 5, [Nenhum]), (Relva, [Nenhum]), (Estrada 4, [Nenhum]), (Relva, [Nenhum]), (Estrada 3, [Nenhum])])]

test6 :: Test
test6 = TestLabel "Testes Tarefa 1" $ test ["Teste 6" ~: True ~=? mapaValido (Mapa 6 [(Rio 1, [Tronco, Tronco, Tronco, Tronco, Tronco, Nenhum]), (Estrada 1, [Carro, Carro, Carro, Nenhum, Nenhum, Nenhum])])]

test7 :: Test
test7 = TestLabel "Testes Tarefa 1" $ test ["Teste 7" ~: False ~=? mapaValido (Mapa 6 [(Rio 1, [Tronco, Tronco, Tronco, Tronco, Tronco, Tronco]), (Estrada 1, [Carro, Carro, Carro, Nenhum, Nenhum, Nenhum])])]

test8 :: Test
test8 = TestLabel "Testes Tarefa 1" $ test ["Teste 8" ~: False ~=? mapaValido (Mapa 6 [(Rio 1, [Tronco, Tronco, Tronco, Tronco, Tronco, Nenhum]), (Estrada 1, [Carro, Carro, Carro, Carro, Nenhum, Nenhum])])]

testsT1 :: Test
testsT1 = TestList [test1,test2,test3,test4,test5,test6,test7,test8]