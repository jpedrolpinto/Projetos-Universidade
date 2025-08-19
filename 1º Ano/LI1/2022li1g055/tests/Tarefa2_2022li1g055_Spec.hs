module Tarefa2_2022li1g055_Spec where

import LI12223
import Tarefa2_2022li1g055
import Test.HUnit



test1 :: Test
test1 = TestLabel "Testes Tarefa 2" $ test ["Teste 1" ~: (Mapa 5 [(Rio (-2),[Nenhum,Nenhum,Nenhum,Nenhum,Nenhum]),(Relva,[Arvore,Nenhum,Arvore,Nenhum,Arvore])]) ~=? estendeMapa (Mapa 5 [(Relva, [Arvore, Nenhum, Arvore, Nenhum, Arvore])]) 21]

test2 :: Test
test2 = TestLabel "Testes Tarefa 2" $ test ["Teste 2" ~: (Mapa 2 [(Relva,[Arvore,Nenhum]),(Estrada (-2),[Nenhum,Nenhum]),(Estrada 2,[Carro,Nenhum]),(Estrada 1,[Carro,Nenhum]),(Estrada 2,[Carro,Nenhum]),(Estrada 3,[Carro,Nenhum])]) ~=? estendeMapa (Mapa 2 [(Estrada (-2),[Nenhum,Nenhum]),(Estrada 2, [Carro,Nenhum]), (Estrada 1, [Carro,Nenhum]), (Estrada 2, [Carro,Nenhum]), (Estrada 3, [Carro,Nenhum])]) 65]

test3 :: Test
test3 = TestLabel "Testes Tarefa 2" $ test ["Teste 3" ~: (Mapa 2 [(Estrada 3,[Carro,Nenhum]),(Relva,[Nenhum,Nenhum]),(Relva,[Arvore,Nenhum]),(Relva,[Arvore,Nenhum]),(Relva,[Nenhum,Nenhum]),(Relva,[Nenhum,Arvore])]) ~=? estendeMapa (Mapa 2 [(Relva,[Nenhum,Nenhum]),(Relva, [Arvore,Nenhum]), (Relva, [Arvore,Nenhum]), (Relva, [Nenhum,Nenhum]), (Relva, [Nenhum,Arvore])]) 97]

test4 :: Test
test4 = TestLabel "Testes Tarefa 2" $ test ["Teste 4" ~: (Mapa 3 [(Estrada (-1),[Carro,Carro,Nenhum]),(Rio (-2),[Nenhum,Tronco,Nenhum]),(Rio 3,[Tronco,Nenhum,Nenhum]),(Rio (-1),[Tronco,Tronco,Nenhum]),(Rio 2,[Tronco,Nenhum,Tronco])]) ~=? estendeMapa (Mapa 3 [(Rio (-2),[Nenhum,Tronco,Nenhum]),(Rio 3,[Tronco,Nenhum,Nenhum]),(Rio (-1),[Tronco,Tronco,Nenhum]), (Rio 2,[Tronco,Nenhum,Tronco])]) 15]

test5 :: Test
test5 = TestLabel "Testes Tarefa 2" $ test ["Teste 5" ~: (Mapa 7 [(Relva,[Arvore,Arvore,Arvore,Arvore,Arvore,Arvore,Nenhum]),(Rio (-2),[Nenhum,Tronco,Nenhum,Tronco,Nenhum,Tronco,Nenhum]),(Relva,[Arvore,Nenhum,Arvore,Nenhum,Arvore,Nenhum,Nenhum])]) ~=? estendeMapa (Mapa 7 [(Rio (-2),[Nenhum,Tronco,Nenhum,Tronco,Nenhum,Tronco,Nenhum]),(Relva,[Arvore,Nenhum,Arvore,Nenhum,Arvore,Nenhum,Nenhum])]) 75]

test6 :: Test
test6 = TestLabel "Testes Tarefa 2" $ test ["Teste 6" ~: (Mapa 7 [(Estrada 1,[Carro,Carro,Carro,Nenhum,Carro,Carro,Carro]),(Rio (-2),[Nenhum,Tronco,Nenhum,Tronco,Nenhum,Tronco,Nenhum]),(Relva,[Arvore,Nenhum,Arvore,Nenhum,Arvore,Nenhum,Nenhum])]) ~=? estendeMapa (Mapa 7 [(Rio (-2),[Nenhum,Tronco,Nenhum,Tronco,Nenhum,Tronco,Nenhum]),(Relva,[Arvore,Nenhum,Arvore,Nenhum,Arvore,Nenhum,Nenhum])]) 65]

testsT2 = TestList [test1,test2,test3,test4,test5,test6]