#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// Función para estimar Pi utilizando el método de Monte Carlo Needle
double monte_carlo_needle(int num_needles) {
    int i, count = 0;
    double pi_estimate;

    // Generar números aleatorios y calcular la estimación de Pi
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

    // Medir el tiempo de ejecución del kernel
    clock_t start_time = clock();

    // Crear procesos hijos
    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid == 0) { // Proceso hijo
            double partial_estimate = monte_carlo_needle(num_needles / num_processes);
            //printf("Proceso hijo %d: Estimaci�n parcial de Pi: %lf\n", getpid(), partial_estimate);
            exit(0);
        } else if (pid < 0) { // Error en fork()
            fprintf(stderr, "Error en fork()\n");
            return 1;
        }
    }

    // Esperar a que todos los procesos hijos terminen
    int status;
    for (int i = 0; i < num_processes; i++) {
        wait(&status);
    }

    // Calcular la estimación final de Pi promediando las estimaciones parciales
    double pi_estimate = 0.0;
    for (int i = 0; i < num_processes; i++) {
        pi_estimate += monte_carlo_needle(num_needles / num_processes);
    }
    pi_estimate /= num_processes;

    // Calcular el tiempo de ejecución del kernel en segundos
    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Estimación de Pi usando Monte Carlo Needle: %lf\n", pi_estimate);
    printf("Tiempo de ejecución del kernel: %lf segundos\n", cpu_time_used);

    return 0;
}
