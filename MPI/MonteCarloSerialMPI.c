#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (argc != 2) {
        if (my_rank == 0) {
            printf("Uso: %s numero_de_agujas\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int num_needles = atoi(argv[1]);
    int num_threads = omp_get_max_threads();

    // Distribuir las agujas entre los procesos de manera más uniforme
    int num_needles_per_process = num_needles / num_processes;
    int remainder = num_needles % num_processes;
    int num_needles_for_this_process = num_needles_per_process;
    if (my_rank < remainder) {
        num_needles_for_this_process++;
    }

    // Inicializar la semilla aleatoria de manera única para cada proceso
    srand(time(NULL) + my_rank);

    // Medir el tiempo de inicio
    double start_time = MPI_Wtime();

    // Contar las agujas que cruzan la línea
    int num_needles_inside = 0;
    for (int i = 0; i < num_needles_for_this_process; i++) {
        double needle_length = ((double)rand() / RAND_MAX) * 2.0;
        double needle_center = ((double)rand() / RAND_MAX) * 1.0;

        if (needle_center - (needle_length / 2) < 0.0 || needle_center + (needle_length / 2) > 1.0) {
            num_needles_inside++;
        }
    }

    // Calcular una estimación de π usando la fórmula de Monte Carlo
    double pi_estimate = (2.0 * num_needles) / num_needles_inside;

    // Medir el tiempo de finalización
    double end_time = MPI_Wtime();

    // Reducir las estimaciones de Pi de todos los procesos
    int num_needles_inside_global;
    double pi_estimate_global;

    MPI_Reduce(&num_needles_inside, &num_needles_inside_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&pi_estimate, &pi_estimate_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Reportar los resultados solo desde el proceso 0
    if (my_rank == 0) {
        pi_estimate_global /= num_needles;
        printf("Estimación de Pi usando Monte Carlo Needles: %lf\n", pi_estimate_global);
        printf("Tiempo de ejecución del kernel: %lf segundos\n", (end_time - start_time));
        printf("Número de procesos utilizados: %d\n", num_processes);
    }

    MPI_Finalize();

    return 0;
}

