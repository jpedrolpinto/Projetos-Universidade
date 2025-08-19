module Tarefa4_2022li1g055_Spec where

import LI12223
import Tarefa4_2022li1g055
import Test.HUnit

test1 :: Test
test1 = TestLabel "Testes Tarefa 4" $ test ["Teste 1" ~: True ~=? jogoTerminou (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada (-2),[Carro,Nenhum]),(Estrada 2, [Carro,Nenhum]), (Estrada 1, [Carro,Nenhum]), (Estrada 2, [Carro,Nenhum]), (Rio 3, [Carro,Nenhum])]))]

test2 :: Test
test2 = TestLabel "Testes Tarefa 4" $ test ["Teste 2" ~: True ~=? jogoTerminou (Jogo (Jogador (1, 0)) (Mapa 2 [(Rio (-2),[Tronco,Nenhum]),(Estrada 2, [Carro,Nenhum]), (Estrada 1, [Carro,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Tronco,Nenhum])]))]

test3 :: Test
test3 = TestLabel "Testes Tarefa 4" $ test ["Teste 3" ~: True ~=? jogoTerminou (Jogo (Jogador (3, 4)) (Mapa 2 [(Rio (-2),[Tronco,Nenhum]),(Estrada 2, [Carro,Nenhum]), (Estrada 1, [Carro,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Tronco,Nenhum])]))]

test4 :: Test
test4 = TestLabel "Testes Tarefa 4" $ test ["Teste 4" ~: False ~=? jogoTerminou (Jogo (Jogador (0, 0)) (Mapa 2 [(Rio (-2),[Tronco,Nenhum]),(Estrada 2, [Carro,Nenhum]), (Estrada 1, [Carro,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Tronco,Nenhum])]))]

test5 :: Test
test5 = TestLabel "Testes Tarefa 4" $ test ["Teste 5" ~: False ~=? jogoTerminou (Jogo (Jogador (1, 0)) (Mapa 2 [(Estrada (-2),[Carro,Nenhum]),(Estrada 2, [Carro,Nenhum]), (Estrada 1, [Carro,Nenhum]), (Estrada 2, [Carro,Nenhum]), (Rio 3, [Carro,Nenhum])]))]

testsT4 = TestList [test1,test2,test3,test4,test5]
