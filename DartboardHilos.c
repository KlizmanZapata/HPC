#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 32

struct ThreadData {
    int num_darts;      // Cantidad total de dardos
    int num_threads;    // Cantidad de hilos
    int *hits;          // Puntero para llevar un registro de los dardos dentro del círculo
};

// Función para estimar Pi usando el algoritmo de Dartboard
void *dartboard(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    int num_darts = data->num_darts;
    int num_threads = data->num_threads;

    // Semilla para la generación de números aleatorios
    unsigned int seed = (unsigned int)time(NULL) * pthread_self();

    // Contador para llevar un registro de los dardos dentro del círculo
    int hits = 0;

    // Generación y verificación de los dardos
    for (int i = 0; i < num_darts / num_threads; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x * x + y * y < 1.0) {
            hits++;
        }
    }

    // Actualizar el contador total de dardos dentro del círculo
    *(data->hits) += hits;

    pthread_exit(NULL);
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

    // Arreglo de hilos
    pthread_t threads[MAX_THREADS];
    // Datos para pasar a los hilos
    struct ThreadData thread_data[MAX_THREADS];
    // Contador total de dardos dentro del círculo
    int total_hits = 0;

    // Inicialización de hilos y generación de números aleatorios
    srand(time(NULL));

    // Creación de hilos y ejecución de la estimación
    clock_t start_time = clock();

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].num_darts = num_darts;
        thread_data[i].num_threads = num_threads;
        thread_data[i].hits = &total_hits;

        pthread_create(&threads[i], NULL, dartboard, (void *)&thread_data[i]);
    }

    // Espera a que todos los hilos terminen
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cálculo de Pi
    double pi_estimate = (double)(total_hits * 4) / num_darts;

    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Estimación de Pi usando el Algoritmo de Dartboard: %lf\n", pi_estimate);
    printf("Tiempo de ejecución del kernel: %lf segundos\n", cpu_time_used);

    return 0;
}
