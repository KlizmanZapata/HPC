#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

#define MAX_THREADS 32

struct ThreadData {
  int num_needles; // Número total de agujas
  int num_threads; // Número de hilos
  int hits;        // Cuenta de agujas dentro del círculo
};

void monte_carlo(struct ThreadData *data, int my_rank) {
  int num_needles = data->num_needles;
  int num_threads = data->num_threads;

  unsigned int seed = (unsigned int)time(NULL) ^ (my_rank + omp_get_thread_num());
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
    printf("Por favor, ingrese un número válido de hilos (1-%d) y agujas (>0).\n", MAX_THREADS);
    return 1;
  }

  struct ThreadData thread_data;
  thread_data.num_needles = num_needles;
  thread_data.num_threads = num_threads;
  thread_data.hits = 0;

  srand(time(NULL));

  MPI_Init(&argc, &argv);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  clock_t start_time = MPI_Wtime();

  monte_carlo(&thread_data, my_rank);

  int global_hits;
  MPI_Reduce(&thread_data.hits, &global_hits, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  double pi_estimate = (double)(global_hits * 4) / num_needles;

  clock_t end_time = MPI_Wtime();
  double cpu_time_used = end_time - start_time;

  if (my_rank == 0) {
    printf("Estimación de Pi usando Monte Carlo Needles: %lf\n", pi_estimate);
    printf("Tiempo de ejecución del kernel: %lf segundos\n", cpu_time_used);
  }

  MPI_Finalize();

  return 0;
}

