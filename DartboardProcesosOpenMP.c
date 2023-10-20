#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define MAX_POINTS 100000000000

double estimate_pi(int total_points) {
    int inside_circle = 0;
    unsigned int seed = (unsigned int)time(NULL) ^ omp_get_thread_num();

    #pragma omp parallel for reduction(+:inside_circle)
    for (int i = 0; i < total_points; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (sqrt(x * x + y * y) <= 1.0) {
            inside_circle++;
        }
    }

    return (double)inside_circle / total_points * 4;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s num_puntos num_procesos\n", argv[0]);
        return 1;
    }

    int total_points = atoi(argv[1);
    int num_processes = atoi(argv[2);

    if (total_points > MAX_POINTS) {
        printf("El número de puntos no debe exceder %d.\n", MAX_POINTS);
        return 1;
    }

    int points_per_process = total_points / num_processes;
    int remaining_points = total_points % num_processes;

    clock_t start_time = clock();

    double pi_estimate = 0.0;

    #pragma omp parallel for reduction(+:pi_estimate)
    for (int i = 0; i < num_processes; i++) {
        int my_points = points_per_process;
        if (i == num_processes - 1) {
            my_points += remaining_points; // El último proceso maneja los puntos restantes
        }

        double result = estimate_pi(my_points);

        #pragma omp critical
        {
            pi_estimate += result;
        }
    }

    // Calcular el tiempo transcurrido
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Estimación de Pi: %lf\n", pi_estimate);
    printf("Tiempo total: %lf segundos\n", elapsed_time);

    return 0;
}

