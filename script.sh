#!/bin/bash

# Definir el nombre del archivo de salida
output_file="tiemposHilos.doc"

# Borra el archivo de salida si ya existe
rm -f "$output_file"

# Número de repeticiones (en este caso, 10)
num_repetitions=10

# Tamaños de matrices a probar
matrix_sizes="10 100 200 400 600 800 1000 2000"

# Número de hilos
num_threads=4

# Ciclo para realizar las repeticiones
for ((j = 1; j <= num_repetitions; j++)); do
    for i in $matrix_sizes; do
        echo "Ejecutando para matriz de tamaño $i (Repetición $j): $num_threads hilos" >> "$output_file"
        
        # Ejecuta el programa y registra el tiempo de ejecución
        { time ./hilos "$i" 0 "$num_threads"; } 2>> "$output_file"
        
        echo "==========================================" >> "$output_file"
    done
done
