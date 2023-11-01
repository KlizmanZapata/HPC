#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

// Función para estimar Pi utilizando el método de Monte Carlo Needle
double monte_carlo_needle(int num_needles) {
    int i, count = 0;
    double pi_estimate;

    // Generar números aleatorios y calcular la estimación de Pi
    #pragma omp parallel for reduction(+:count)
    for (i = 0; i < num_needles; i++) {
        double x = (double)rand() / RAND_MAX; // Posición X aleatoria
        double theta = (double)rand() / RAND_MAX * M_PI / 2.0; // Ángulo theta aleatorio
        double d = x / 2 * tan(theta); // Distancia d entre aguja y línea

        if (d <= 0.5) {
            count++;
        }
    }

    pi_estimate = (2.0 * num_needles) / (double)count;

    return pi_estimate;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s num_agujas num_procesos\n", argv[0]);
        return 1;
    }

    int num_needles = atoi(argv[1]);
    int num_processes = atoi(argv[2]);

    // Inicializar la semilla para la generación de números aleatorios
    srand(time(NULL));

    // Medir el tiempo de ejecución del kernel
    clock_t start_time = clock();
    
    double pi_estimate = 0.0;

    // Parallelizar el cálculo de la estimación de Pi
    #pragma omp parallel for reduction(+:pi_estimate)
    for (int i = 0; i < num_processes; i++) {
        double partial_estimate = monte_carlo_needle(num_needles / num_processes);
        pi_estimate += partial_estimate;
    }

    // Calcular la estimación final de Pi promediando las estimaciones parciales
    pi_estimate /= num_processes;

    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Estimación de Pi usando Monte Carlo Needle: %lf\n", pi_estimate);
    printf("Tiempo de ejecución del kernel: %lf segundos\n", cpu_time_used);

    return 0;
}

