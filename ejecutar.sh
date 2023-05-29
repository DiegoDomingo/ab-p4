#
# AUTORES:     Diego Domingo Ralla (818637)
#              Simón Alonso Gutiérrez (821038)
# ASIGNATURA:  Algoritmia Básica
# FECHA:       29 mayo 2023
# FICHERO:     ejecutar.sh
# DESCRIPCIÓN: Script de ejecución que automatiza la compilación y ejecución del
#              programa con varios casos de prueba
#

#!/bin/bash

# Compilación del fuente
g++ -std=c++11 transporte.cc -o transporte

# Ejecución del programa
./transporte pruebas.txt resultados.txt

# Limpieza de ejecutables
rm transporte