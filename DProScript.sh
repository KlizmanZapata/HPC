#!/bin/bash

# Definir el nombre del archivo de salida
output_file="tiemposMonteProcesos1millon.txt"

# Borra el archivo de salida si ya existe
rm -f "$output_file"

# Números de agujas a probar
needle_counts="1000000 100000000 1000000000 10000000000 100000000000"

# Números de procesos a probar
num_processes="2 4 8 16 32"

# Número de repeticiones (en este caso, 10)
num_repetitions=5

# Ruta al programa que deseas ejecutar
program="./DartboardProcesos"  # Reemplaza esto con la ruta de tu programa

# Ciclo para realizar las repeticiones
for ((j = 1; j <= num_repetitions; j++)); do
    for needles in $needle_counts; do
        for processes in $num_processes; do
            echo "Ejecutando con $processes procesos y $needles agujas (Repetición $j)" >> "$output_file"

            # Ejecuta el programa y registra el tiempo de ejecución y la estimación de Pi
            output=$($program "$needles" "$processes")
            echo "$output" >> "$output_file"

            echo "==========================================" >> "$output_file"
        done
    done
done


