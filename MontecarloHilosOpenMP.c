#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX_THREADS 32

struct ThreadData {
    int num_needles;   // Cantidad total de agujas
    int num_threads;   // Cantidad de hilos
    int hits;          // Cantidad de agujas dentro del círculo
};

void monte_carlo(struct ThreadData *data) {
    int num_needles = data->num_needles;
    int num_threads = data->num_threads;

    unsigned int seed = (unsigned int)time(NULL) ^ omp_get_thread_num();
    int hits = 0;

    #pragma omp parallel for reduction(+:hits)
    for (int i = 0; i < num_needles / num_threads; i++) {
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
        printf("Uso: %s num_hilos num_agujas\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int num_needles = atoi(argv[2]);

    if (num_threads < 1 || num_threads > MAX_THREADS || num_needles < 1) {
        printf("Por favor, ingresa un número válido de hilos (1-%d) y agujas (>0).\n", MAX_THREADS);
        return 1;
    }

    struct ThreadData thread_data;
    thread_data.num_needles = num_needles;
    thread_data.num_threads = num_threads;
    thread_data.hits = 0;

    srand(time(NULL));

    clock_t start_time = clock();

    monte_carlo(&thread_data);

    double pi_estimate = (double)(thread_data.hits * 4) / num_needles;

    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Estimación de Pi usando Monte Carlo Needles: %lf\n", pi_estimate);
    printf("Tiempo de ejecución del kernel: %lf segundos\n", cpu_time_used);

    return 0;
}

