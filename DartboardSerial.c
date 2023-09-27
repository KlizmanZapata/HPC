#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double estimate_pi_serial(long long total_points) {
    long long points_inside_circle = 0;

    for (long long i = 0; i < total_points; i++) {
        double x = (double)rand() / RAND_MAX; // Generar coordenada x aleatoria entre 0 y 1
        double y = (double)rand() / RAND_MAX; // Generar coordenada y aleatoria entre 0 y 1

        double distance = x * x + y * y;

        if (distance <= 1.0) {
            points_inside_circle++;
        }
    }

    return 4.0 * ((double)points_inside_circle / total_points);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s numero_de_puntos\n", argv[0]);
        return 1;
    }

    long long total_points = atoll(argv[1]);

    srand(time(NULL)); // Inicializar la semilla para generar números aleatorios

    clock_t start_time = clock();

    double pi_estimate = estimate_pi_serial(total_points);

    clock_t end_time = clock();
    double cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Estimación de Pi usando Monte Carlo Dartboard: %lf\n", pi_estimate);
    printf("Tiempo de ejecución del kernel: %lf segundos\n", cpu_time_used);

    return 0;
}
