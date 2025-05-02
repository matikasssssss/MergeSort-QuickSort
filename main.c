#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "diskio.h"

void mergesort_externo(const char *archivo_entrada, const char *archivo_salida, size_t n);
void quicksort_externo(const char *archivo_entrada, const char *archivo_salida, size_t n);
void generar_datos_binarios(const char *archivo, size_t n);

double medir_tiempo(void (*func)(const char *, const char *, size_t),
                    const char *in, const char *out, size_t n,
                    size_t *lecturas, size_t *escrituras)
{
    cont_lecturas = 0;
    cont_escrituras = 0;

    clock_t inicio = clock();
    func(in, out, n);
    clock_t fin = clock();

    *lecturas = cont_lecturas;
    *escrituras = cont_escrituras;

    return (double)(fin - inicio) / CLOCKS_PER_SEC;
}

int main()
{
    size_t tamanos[] = {4, 8, 16, 32, 60};
    size_t n;
    char nombre_in[64], nombre_out[64];

    for (int i = 0; i < 5; i++)
    {
        n = tamanos[i] * 1000000ULL;

        snprintf(nombre_in, sizeof(nombre_in), "input_%luM.bin", tamanos[i]);
        snprintf(nombre_out, sizeof(nombre_out), "mergeout_%luM.bin", tamanos[i]);

        printf("Generando archivo de entrada %s...\n", nombre_in);
        generar_datos_binarios(nombre_in, n);

        printf("Corriendo Mergesort externo...\n");
        size_t r, w;
        double t = medir_tiempo(mergesort_externo, nombre_in, nombre_out, n, &r, &w);
        printf("Tiempo: %.2fs, Lecturas: %lu, Escrituras: %lu\n", t, r, w);

        snprintf(nombre_out, sizeof(nombre_out), "quickout_%luM.bin", tamanos[i]);
        printf("Corriendo Quicksort externo...\n");
        t = medir_tiempo(quicksort_externo, nombre_in, nombre_out, n, &r, &w);
        printf("Tiempo: %.2fs, Lecturas: %lu, Escrituras: %lu\n", t, r, w);
    }

    return 0;
}
