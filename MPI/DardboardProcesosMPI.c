#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

#define MAX_PUNTOS 100000000000

double estimate_pi_mpi(int total_puntos, int my_rank) {
  int dentro_del_circulo = 0;
  unsigned int seed = (unsigned int)time(NULL) ^ omp_get_thread_num() ^ my_rank;

  #pragma omp parallel for reduction(+:dentro_del_circulo)
  for (int i = 0; i < total_puntos; i++) {
    double x = (double)rand_r(&seed) / RAND_MAX;
    double y = (double)rand_r(&seed) / RAND_MAX;

    if (sqrt(x * x + y * y) <= 1.0) {
      dentro_del_circulo++;
    }
  }

  int global_dentro_del_circulo;
  MPI_Reduce(&dentro_del_circulo, &global_dentro_del_circulo, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  double pi_estimada;
  if (my_rank == 0) {
    pi_estimada = (double)global_dentro_del_circulo / total_puntos * 4;
  }

  return pi_estimada;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Uso: %s num_puntos num_procesos\n", argv[0]);
    return 1;
  }

  int total_puntos = atoi(argv[1]);
  int num_procesos = atoi(argv[2]);

  if (total_puntos > MAX_PUNTOS) {
    printf("El número de puntos no debe exceder %ld.\n", MAX_PUNTOS);
    return 1;
  }

  MPI_Init(&argc, &argv);
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  int num_procs;
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  if (num_procs != num_procesos) {
    if (my_rank == 0) {
      printf("El número de procesos especificado no coincide con el número de procesos MPI.\n");
    }
    MPI_Finalize();
    return 1;
  }

  int puntos_por_proceso = total_puntos / num_procesos;

  clock_t start_time = MPI_Wtime();

  double pi_estimada = estimate_pi_mpi(puntos_por_proceso, my_rank);

  clock_t end_time = MPI_Wtime();
  double tiempo_transcurrido = end_time - start_time;

  double global_pi_estimada;
  MPI_Reduce(&pi_estimada, &global_pi_estimada, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    printf("Estimación de Pi: %lf\n", global_pi_estimada / num_procesos);
    printf("Tiempo total: %lf segundos\n", tiempo_transcurrido);
  }

  MPI_Finalize();

  return 0;
}

