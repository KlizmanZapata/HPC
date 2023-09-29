#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_POINTS 100000000000

double estimate_pi(int total_points) {
    int inside_circle = 0;

    unsigned int seed = time(NULL);

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

    int total_points = atoi(argv[1]);
    int num_processes = atoi(argv[2]);

    if (total_points > MAX_POINTS) {
        printf("El número de puntos no debe exceder %ld.\n", MAX_POINTS);
        return 1;
    }

    int points_per_process = total_points / num_processes;
    int remaining_points = total_points % num_processes;

    pid_t *child_pids = (pid_t *)malloc(num_processes * sizeof(pid_t));

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            int my_points = points_per_process;
            if (i == num_processes - 1) {
                my_points += remaining_points; // El último proceso maneja los puntos restantes
            }

            double result = estimate_pi(my_points);

            // printf("Proceso %d estimó Pi como: %lf\n", getpid(), result);

            exit(0);
        } else if (pid < 0) {
            fprintf(stderr, "Error en fork()\n");
            return 1;
        } else {
            child_pids[i] = pid;
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < num_processes; i++) {
        waitpid(child_pids[i], NULL, 0);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    double pi_estimate = estimate_pi(total_points);
    printf("Estimación final de Pi: %lf\n", pi_estimate);

    printf("Tiempo de ejecución del kernel: %lf segundos\n", elapsed_time);

    free(child_pids);

    return 0;
}

