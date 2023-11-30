#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

double estimate_pi_openmp_mpi(long long total_points, int my_rank) {
  long long points_inside_circle = 0;

  #pragma omp parallel
  {
    unsigned int seed = time(NULL) ^ omp_get_thread_num() ^ my_rank;
    #pragma omp for reduction(+:points_inside_circle)
    for (long long i = 0; i < total_points; i++) {
      double x = (double)rand_r(&seed) / RAND_MAX; // Generar coordenada x aleatoria entre 0 y 1
      double y = (double)rand_r(&seed) / RAND_MAX; // Generar coordenada y aleatoria entre 0 y 1

      double distance = x * x + y * y;

      if (distance <= 1.0) {
        points_inside_circle++;
      }
    }
  }

  long long global_points_inside_circle;
  MPI_Reduce(&points_inside_circle, &global_points_inside_circle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

  double pi_estimate;
  if (my_rank == 0) {
    pi_estimate = 4.0 * ((double)global_points_inside_circle / (total_points * omp_get_max_threads()));
  }

  return pi_estimate;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Uso: %s numero_de_puntos\n", argv[0]);
    return 1;
  }

  long long total_points = atoll(argv[1]);

  MPI_Init(&argc, &argv);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  int num_processes;
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

  srand(time(NULL)); // Inicializar la semilla para generar nÃºmeros aleatorios

  double pi_estimate = estimate_pi_openmp_mpi(total_points / num_processes, my_rank);

  double global_pi_estimate;
  MPI_Reduce(&pi_estimate, &global_pi_estimate, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    printf("EstimaciÃ³n de Pi usando Monte Carlo Dartboard: %lf\n", global_pi_estimate / num_processes);
  }

  MPI_Finalize();

  return 0;
}
