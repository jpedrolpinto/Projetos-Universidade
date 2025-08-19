module Tarefa3_2022li1g055_Spec where

import LI12223
import Tarefa3_2022li1g055
import Test.HUnit

test1 :: Test
test1 = TestLabel "Testes Tarefa 3" $ test ["Teste 1" ~: (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Nenhum,Tronco])])) ~=? animaJogo (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Tronco,Nenhum])])) (Move Baixo)]

test2 :: Test
test2 = TestLabel "Testes Tarefa 3" $ test ["Teste 2" ~: (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Nenhum,Tronco])])) ~=? animaJogo (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Tronco,Nenhum])])) (Move Cima)]

test3 :: Test
test3 = TestLabel "Testes Tarefa 3" $ test ["Teste 3" ~: (Jogo (Jogador (1, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Nenhum,Tronco])])) ~=? animaJogo (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Tronco,Nenhum])])) (Move Direita)]

test4 :: Test
test4 = TestLabel "Testes Tarefa 3" $ test ["Teste 4" ~: (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Nenhum,Tronco])])) ~=? animaJogo (Jogo (Jogador (1, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Arvore,Nenhum]), (Rio 3, [Tronco,Nenhum])])) (Move Esquerda)]

test5 :: Test
test5 = TestLabel "Testes Tarefa 3" $ test ["Teste 5" ~: (Jogo (Jogador (1, 0)) (Mapa 3 [(Rio 1, [Nenhum,Tronco,Nenhum]), (Relva, [Nenhum,Nenhum,Arvore]), (Rio 3, [Nenhum,Tronco,Nenhum])])) ~=? animaJogo (Jogo (Jogador (0, 0)) (Mapa 3 [(Rio 1, [Tronco,Nenhum,Nenhum]), (Relva, [Nenhum,Nenhum,Arvore]), (Rio 3, [Nenhum,Tronco,Nenhum])])) (Parado)]

test6 :: Test
test6 = TestLabel "Testes Tarefa 3" $ test ["Teste 6" ~: (Jogo (Jogador (0, 1)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Nenhum,Arvore]), (Rio 3, [Nenhum,Tronco])])) ~=? animaJogo (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Nenhum,Arvore]), (Rio 3, [Tronco,Nenhum])])) (Move Baixo)]

test7 :: Test
test7 = TestLabel "Testes Tarefa 3" $ test ["Teste 7" ~: (Jogo (Jogador (0, 0)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Nenhum,Arvore]), (Rio 3, [Nenhum,Tronco])])) ~=? animaJogo (Jogo (Jogador (0, 1)) (Mapa 2 [(Estrada 1, [Nenhum,Nenhum]), (Relva, [Nenhum,Arvore]), (Rio 3, [Tronco,Nenhum])])) (Move Cima)]


testsT3 = TestList [test1,test2,test3,test4,test5,test6,test7]