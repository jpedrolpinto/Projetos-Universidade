{- |
Module      : Tarefa6_2022li1g055
Description : 
Copyright   : João Pedro Loureiro Pinto <a104270@alunos.uminho.pt>
              Diogo Alexandre Gomes Silva <a104183@alunos.uminho.pt>

Módulo para a realização da Tarefa 6 do projeto de LI1 em 2022/23.
-}


module Main where

import Graphics.Gloss
import Graphics.Gloss.Interface.Pure.Game

import Tarefa1_2022li1g055
import Tarefa2_2022li1g055
import Tarefa3_2022li1g055
import Tarefa4_2022li1g055
import Tarefa5_2022li1g055

type Estado = (Float, Float)
type EstadoGloss = (Estado, Picture)

estadoInicial :: Estado
estadoInicial = (0,0)

estadoGlossInicial :: Picture -> EstadoGloss
estadoGlossInicial z = (estadoInicial, z)

reageEventoGloss :: Event -> EstadoGloss -> EstadoGloss
reageEventoGloss (EventKey (SpecialKey KeyUp) Down _ _)  ((x,y), z) = ((x,y+20), z)
reageEventoGloss (EventKey (SpecialKey KeyDown) Down _ _)  ((x,y), z) = ((x,y-20), z)
reageEventoGloss (EventKey (SpecialKey KeyLeft) Down _ _)  ((x,y), z) = ((x+20,y), z)
reageEventoGloss (EventKey (SpecialKey KeyRight) Down _ _)  ((x,y), z) = ((x-20,y), z)
reageEventoGloss _ s = s


reageTempoGloss :: Float -> EstadoGloss -> EstadoGloss
reageTempoGloss n ((x,y), z) = ((x,y+1), z)

fr :: Int
fr = 50

dm :: Display
dm = InWindow "CrossyPombo" (400, 400) (0, 0)

desenhaEstadoGloss :: EstadoGloss -> Picture
desenhaEstadoGloss ((x,y), z) = Translate x y z

main :: IO ()
main = do 
       pombo <- loadBMP "pombo.bmp"
       play dm
           (greyN 0.2)
           fr
           (estadoGlossInicial pombo)
           desenhaEstadoGloss
           reageEventoGloss
           reageTempoGloss 