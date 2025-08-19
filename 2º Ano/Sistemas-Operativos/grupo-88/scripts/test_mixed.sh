#!/bin/bash

# Executar diferentes operações simultaneamente
./dclient -c 1 &
./dclient -l 3 "teste" &
./dclient -s "conteúdo" &
./dclient -a "Documento 7" "Autor 7" "2019" "1112.txt" &
./dclient -d 5 &

# Aguardar a conclusão
wait