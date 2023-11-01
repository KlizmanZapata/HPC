#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s numero_de_agujas\n", argv[0]);
        return 1;
    }

    int num_needles = atoi(argv[1]);
    int num_threads = omp_get_max_threads();

    srand(time(NULL));

    clock_t start_time = clock(); // Inicio de medición de tiempo

    int num_needles_inside = 0;

    #pragma omp parallel for reduction(+:num_needles_inside)
    for (int i = 0; i < num_needles; i++) {
        // Generar una aguja con longitud aleatoria y centro de caída aleatorio.
        double needle_length = ((double)rand() / RAND_MAX) * 2.0;
        double needle_center = ((double)rand() / RAND_MAX) * 1.0;

        // Verificar si la aguja cruza una línea.
        if (needle_center - (needle_length / 2) < 0.0 || needle_center + (needle_length / 2) > 1.0) {
            num_needles_inside++;
        }
    }

    // Calcular una estimación de π usando la fórmula de Monte Carlo.
    double pi_estimate = (2.0 * num_needles) / num_needles_inside;

    clock_t end_time = clock(); // Fin de medición de tiempo
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Estimación de Pi usando Monte Carlo Needles: %lf\n", pi_estimate);
    printf("Tiempo de ejecución del kernel: %lf segundos\n", cpu_time_used);
    printf("Número de hilos utilizados: %d\n", num_threads);

    return 0;
}

