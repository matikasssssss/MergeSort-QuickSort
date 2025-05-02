#include "mergesort.h"
#include "diskio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Intercambia dos elementos en un buffer
void intercambiar_elementos(int64_t *arr, size_t i, size_t j) {
    int64_t tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

// Ordena un bloque en memoria usando quicksort interno
int comparar_int64(const void *a, const void *b) {
    int64_t x = *(int64_t *)a;
    int64_t y = *(int64_t *)b;
    return (x > y) - (x < y);
}

// Crea runs ordenados (cada uno de hasta a bloques), devuelve número de runs
size_t crear_runs_iniciales(FILE *entrada, size_t elementos_totales, size_t a, char **nombres_runs) {
    size_t elementos_por_run = a * ELEMENTS_PER_BLOCK;
    size_t num_runs = (elementos_totales + elementos_por_run - 1) / elementos_por_run;

    int64_t *buffer = malloc(elementos_por_run * sizeof(int64_t));

    for (size_t i = 0; i < num_runs; i++) {
        size_t elementos_a_leer = elementos_por_run;
        if ((i + 1) * elementos_por_run > elementos_totales)
            elementos_a_leer = elementos_totales - i * elementos_por_run;

        fread(buffer, sizeof(int64_t), elementos_a_leer, entrada);
        qsort(buffer, elementos_a_leer, sizeof(int64_t), comparar_int64);

        char *nombre_run = malloc(32);
        snprintf(nombre_run, 32, "run_%zu.bin", i);
        nombres_runs[i] = nombre_run;

        FILE *f_run = fopen(nombre_run, "wb");
        fwrite(buffer, sizeof(int64_t), elementos_a_leer, f_run);
        fclose(f_run);
    }

    free(buffer);
    return num_runs;
}

// Mezcla a archivos ordenados y genera un archivo de salida ordenado
void mezclar_archivos(char **input_names, size_t count, const char *output_name) {
    FILE **inputs = malloc(sizeof(FILE *) * count);
    int64_t *buffer = malloc(sizeof(int64_t) * count);
    int activo[count];
    size_t pos[count];

    for (size_t i = 0; i < count; i++) {
        inputs[i] = fopen(input_names[i], "rb");
        fread(&buffer[i], sizeof(int64_t), 1, inputs[i]);
        activo[i] = 1;
        pos[i] = 1;
    }

    FILE *out = fopen(output_name, "wb");

    while (1) {
        int min_idx = -1;
        for (size_t i = 0; i < count; i++) {
            if (activo[i]) {
                if (min_idx == -1 || buffer[i] < buffer[min_idx]) {
                    min_idx = i;
                }
            }
        }

        if (min_idx == -1)
            break; // todos terminados

        fwrite(&buffer[min_idx], sizeof(int64_t), 1, out);

        if (fread(&buffer[min_idx], sizeof(int64_t), 1, inputs[min_idx]) != 1)
            activo[min_idx] = 0;
    }

    for (size_t i = 0; i < count; i++) {
        fclose(inputs[i]);
    }

    fclose(out);
    free(inputs);
    free(buffer);
}

// Mezcla recursiva de runs usando aridad a
void mezclar_runs(char **nombres_runs, size_t num_runs, size_t a, int nivel) {
    if (num_runs == 1) {
        rename(nombres_runs[0], "entrada_tmp.bin");
        return;
    }

    size_t nuevos_runs = (num_runs + a - 1) / a;
    char **nuevos_nombres = malloc(sizeof(char *) * nuevos_runs);

    for (size_t i = 0; i < nuevos_runs; i++) {
        size_t inicio = i * a;
        size_t fin = (i + 1) * a;
        if (fin > num_runs) fin = num_runs;

        char nombre_out[32];
        snprintf(nombre_out, 32, "run_nivel%d_%zu.bin", nivel, i);
        nuevos_nombres[i] = strdup(nombre_out);

        mezclar_archivos(&nombres_runs[inicio], fin - inicio, nombre_out);
    }

    for (size_t i = 0; i < num_runs; i++) {
        remove(nombres_runs[i]);
        free(nombres_runs[i]);
    }

    mezclar_runs(nuevos_nombres, nuevos_runs, a, nivel + 1);

    for (size_t i = 0; i < nuevos_runs; i++) {
        free(nuevos_nombres[i]);
    }
    free(nuevos_nombres);
}

// Función principal
void mergesort_externo(const char *nombre_archivo, size_t M, size_t a) {
    FILE *entrada = fopen(nombre_archivo, "rb");
    if (!entrada) {
        perror("fopen");
        exit(1);
    }

    fseek(entrada, 0, SEEK_END);
    size_t tam_archivo = ftell(entrada);
    size_t elementos_totales = tam_archivo / sizeof(int64_t);
    rewind(entrada);

    size_t max_runs = (elementos_totales + (a * ELEMENTS_PER_BLOCK - 1)) / (a * ELEMENTS_PER_BLOCK);
    char **nombres_runs = malloc(sizeof(char *) * max_runs);

    size_t num_runs = crear_runs_iniciales(entrada, elementos_totales, a, nombres_runs);
    fclose(entrada);

    mezclar_runs(nombres_runs, num_runs, a, 0);

    free(nombres_runs);
}
