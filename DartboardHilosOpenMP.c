#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX_THREADS 32

struct ThreadData {
    int num_darts;      // Cantidad total de dardos
    int num_threads;    // Cantidad de hilos
    int hits;           // Cantidad de dardos dentro del círculo
};

// Función para estimar Pi usando el algoritmo de Dartboard
void dartboard(struct ThreadData *data) {
    int num_darts = data->num_darts;
    int num_threads = data->num_threads;

    // Semilla para la generación de números aleatorios
    unsigned int seed = (unsigned int)time(NULL) * omp_get_thread_num();

    // Contador para llevar un registro de los dardos dentro del círculo
    int hits = 0;

    // Generación y verificación de los dardos
    #pragma omp parallel for reduction(+:hits)
    for (int i = 0; i < num_darts / num_threads; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x * x + y * y < 1.0) {
            hits++;
        }
    }

    #pragma omp critical
    {
        data->hits += hits;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s num_hilos num_dardos\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int num_darts = atoi(argv[2]);

    if (num_threads < 1 || num_threads > MAX_THREADS || num_darts < 1) {
        printf("Por favor, ingresa un número válido de hilos (1-%d) y dardos (>0).\n", MAX_THREADS);
        return 1;
    }

    struct ThreadData thread_data;
    thread_data.num_darts = num_darts;
    thread_data.num_threads = num_threads;
    thread_data.hits = 0;

    // Inicialización de hilos y generación de números aleatorios
    srand(time(NULL));

    // Medir el tiempo de ejecución del kernel
    clock_t start_time = clock();

    dartboard(&thread_data);

    // Cálculo de Pi
    double pi_estimate = (double)(thread_data.hits * 4) / num_darts;

    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Estimación de Pi usando el Algoritmo de Dartboard: %lf\n", pi_estimate);
    printf("Tiempo de ejecución del kernel: %lf segundos\n", cpu_time_used);

    return 0;
}
