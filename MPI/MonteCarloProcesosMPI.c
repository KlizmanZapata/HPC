#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

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
    MPI_Init(&argc, &argv);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (argc != 3) {
        if (my_rank == 0) {
            printf("Uso: %s num_agujas num_procesos\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int num_needles = atoi(argv[1]);

    // Inicializar la semilla para la generación de números aleatorios
    srand(time(NULL) + my_rank);

    // Distribuir las agujas entre los procesos de manera más uniforme
    int num_needles_per_process = num_needles / num_processes;
    int remainder = num_needles % num_processes;
    int num_needles_for_this_process = num_needles_per_process;
    if (my_rank < remainder) {
        num_needles_for_this_process++;
    }

    // Inicializar la estimación de Pi para cada proceso
    double pi_estimate = 0.0;
    double pi_estimate_global = 0.0;

    // Calcular la estimación de Pi para cada proceso
    for (int i = 0; i < num_needles_for_this_process; i++) {
        double partial_estimate = monte_carlo_needle(num_needles_per_process);
        pi_estimate += partial_estimate;
    }

    // Reducir las estimaciones de Pi de todos los procesos
    MPI_Reduce(&pi_estimate, &pi_estimate_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Calcular la estimación final de Pi solo en el proceso 0
    if (my_rank == 0) {
        pi_estimate_global /= num_needles;
    }

    // Reportar los resultados solo desde el proceso 0
    if (my_rank == 0) {
        printf("Estimación de Pi usando Monte Carlo Needle: %lf\n", pi_estimate_global);
    }

    MPI_Finalize();

    return 0;
}

