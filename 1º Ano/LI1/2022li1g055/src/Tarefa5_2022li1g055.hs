{- |
Module      : Tarefa5_2022li1g055
Description : Deslize do Jogo
Copyright   : João Pedro Loureiro Pinto <a104270@alunos.uminho.pt>
              Diogo Alexandre Gomes Silva <a104183@alunos.uminho.pt>

Módulo para a realização da Tarefa 5 do projeto de LI1 em 2022/23.
-}
module Tarefa5_2022li1g055 where

import LI12223
import Tarefa2_2022li1g055

deslizaJogo :: Int -> Jogo -> Jogo
deslizaJogo n (Jogo (Jogador (x, y)) (Mapa l t)) = Jogo (Jogador (x, y+1)) (estendeMapa ( retiraUltimaLinha (Mapa l t)) n ) 

retiraUltimaLinha :: Mapa -> Mapa
retiraUltimaLinha (Mapa l t) = Mapa l (init t)