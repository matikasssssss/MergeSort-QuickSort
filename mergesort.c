#include "mergesort.h"
#include "diskio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void intercambiar_elementos(int64_t *arr, size_t i, size_t j)
{
    int64_t tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

int comparar_int64(const void *a, const void *b)
{
    int64_t x = *(int64_t *)a;
    int64_t y = *(int64_t *)b;
    return (x > y) - (x < y);
}

size_t crear_runs_iniciales(FILE *entrada, size_t elementos_totales, size_t a, char **nombres_runs)
{
    size_t elementos_por_run = a * ELEMENTS_PER_BLOCK;
    size_t num_runs = (elementos_totales + elementos_por_run - 1) / elementos_por_run;

    int64_t *buffer = malloc(elementos_por_run * sizeof(int64_t));
    
    

    for (size_t i = 0; i < num_runs; i++)
    {
        size_t elementos_a_leer = elementos_por_run;
        if ((i + 1) * elementos_por_run > elementos_totales)
            elementos_a_leer = elementos_totales - i * elementos_por_run;

        size_t bloques_a_leer = (elementos_a_leer + ELEMENTS_PER_BLOCK - 1) / ELEMENTS_PER_BLOCK;
        size_t bloque_inicio = i * a;

        for (size_t j = 0; j < bloques_a_leer; j++)
        {
            size_t offset = j * ELEMENTS_PER_BLOCK;
            size_t restantes = elementos_a_leer - offset;
            size_t cantidad = restantes >= ELEMENTS_PER_BLOCK ? ELEMENTS_PER_BLOCK : restantes;

            leer_bloque(entrada, buffer + offset, bloque_inicio + j, cantidad);
        }

        qsort(buffer, elementos_a_leer, sizeof(int64_t), comparar_int64);

        char *nombre_run = malloc(32);
        snprintf(nombre_run, 32, "run_%zu.bin", i);
        nombres_runs[i] = nombre_run;

        FILE *f_run = fopen(nombre_run, "wb");
        for (size_t j = 0; j < bloques_a_leer; j++)
        {
            size_t offset = j * ELEMENTS_PER_BLOCK;
            size_t restantes = elementos_a_leer - offset;
            size_t cantidad = restantes >= ELEMENTS_PER_BLOCK ? ELEMENTS_PER_BLOCK : restantes;

            escribir_bloque(f_run, buffer + offset, j, cantidad);
        }
        fclose(f_run);
    }

    free(buffer);
    return num_runs;
}



typedef struct
{
    FILE *f;
    int64_t elementos_restantes;
    int64_t *buffer;
    size_t pos;
    size_t elementos_validos;
    size_t bloque_actual;
    int activo;
} RunEntrada;

void mezclar_archivos(char **input_names, size_t count, const char *output_name)
{
    RunEntrada *entradas = malloc(sizeof(RunEntrada) * count);
    size_t tam_bloque = ELEMENTS_PER_BLOCK;

    // Inicializar las entradas
    for (size_t i = 0; i < count; i++)
    {
        entradas[i].f = fopen(input_names[i], "rb");
        fseek(entradas[i].f, 0, SEEK_END);
        size_t tam_archivo = ftell(entradas[i].f);
        rewind(entradas[i].f);

        entradas[i].elementos_restantes = tam_archivo / sizeof(int64_t);
        entradas[i].buffer = malloc(sizeof(int64_t) * tam_bloque);
        
        if (!entradas[i].buffer) {
            perror("Error al asignar memoria");
            exit(1);
        }
        entradas[i].pos = 0;
        entradas[i].bloque_actual = 0;
        entradas[i].activo = 1;


        size_t cantidad = entradas[i].elementos_restantes >= tam_bloque ? tam_bloque : entradas[i].elementos_restantes;
        leer_bloque(entradas[i].f, entradas[i].buffer, entradas[i].bloque_actual, cantidad);
        entradas[i].elementos_validos = cantidad;
        entradas[i].elementos_restantes -= cantidad;
        entradas[i].bloque_actual++;

        size_t leidos = fread(entradas[i].buffer, sizeof(int64_t), tam_bloque, entradas[i].f);
        entradas[i].elementos_validos = leidos;
        if (leidos == 0)
            entradas[i].activo = 0;

    }

    FILE *out = fopen(output_name, "wb");
    int64_t *out_buffer = malloc(sizeof(int64_t) * tam_bloque);
    if (!out_buffer) {
        perror("Error al asignar memoria");
        exit(1);
    }
    size_t out_pos = 0;
    size_t bloques_escritos = 0;

    // Mezcla
    while (1)
    {
        int min_idx = -1;

        for (size_t i = 0; i < count; i++)
        {
            if (entradas[i].activo && entradas[i].pos < entradas[i].elementos_validos)
            {
                if (min_idx == -1 || entradas[i].buffer[entradas[i].pos] < entradas[min_idx].buffer[entradas[min_idx].pos])
                {
                    min_idx = i;
                }
            }
        }

        if (min_idx == -1)
            break;

        out_buffer[out_pos++] = entradas[min_idx].buffer[entradas[min_idx].pos++];

        if (out_pos == tam_bloque)
        {
            escribir_bloque(out, out_buffer, bloques_escritos++, out_pos);
            out_pos = 0;
        }

        if (entradas[min_idx].pos == entradas[min_idx].elementos_validos)
        {

            if (entradas[min_idx].elementos_restantes > 0)

            size_t leidos = fread(entradas[min_idx].buffer, sizeof(int64_t), tam_bloque, entradas[min_idx].f);
            if (leidos == 0)

            {
                size_t cantidad = entradas[min_idx].elementos_restantes >= tam_bloque ? tam_bloque : entradas[min_idx].elementos_restantes;
                leer_bloque(entradas[min_idx].f, entradas[min_idx].buffer, entradas[min_idx].bloque_actual, cantidad);
                entradas[min_idx].elementos_validos = cantidad;
                entradas[min_idx].elementos_restantes -= cantidad;
                entradas[min_idx].pos = 0;
                entradas[min_idx].bloque_actual++;
            }
            else
            {
                entradas[min_idx].activo = 0;
            }
        }
    }

    // Escribir los elementos restantes
    if (out_pos > 0)
    {
        escribir_bloque(out, out_buffer, bloques_escritos++, out_pos);
    }

    // Liberación de recursos
    for (size_t i = 0; i < count; i++)
    {
        fclose(entradas[i].f);
        free(entradas[i].buffer);
    }
    fclose(out);
    free(out_buffer);
    free(entradas);
}






void mezclar_runs(char **nombres_runs, size_t num_runs, size_t a, int nivel)
{
    if (num_runs == 1)
    {
        printf("Nivel %d: archivo final a renombrar: %s\n", nivel, nombres_runs[0]);

        // Verificamos que el archivo exista antes de renombrar
        FILE *test = fopen(nombres_runs[0], "rb");
        if (!test)
        {
            perror("No se puede abrir el archivo final");
            exit(1);
        }
        fclose(test);

        if (rename(nombres_runs[0], "entrada_tmp.bin") != 0)
        {
            perror("Fallo al renombrar archivo final");
            exit(1);
        }

        free(nombres_runs[0]);
        free(nombres_runs);
        return;
    }

    size_t nuevos_runs = (num_runs + a - 1) / a;
    char **nuevos_nombres = malloc(sizeof(char *) * nuevos_runs);

    for (size_t i = 0; i < nuevos_runs; i++)
    {
        size_t inicio = i * a;
        size_t fin = (i + 1) * a;
        if (fin > num_runs)
            fin = num_runs;

        printf("Nivel %d: mezclando archivos del %zu al %zu\n", nivel, inicio, fin);

        char nombre_out[32];
        snprintf(nombre_out, 32, "run_nivel%d_%zu.bin", nivel, i);
        nuevos_nombres[i] = strdup(nombre_out);

        mezclar_archivos(&nombres_runs[inicio], fin - inicio, nombre_out);

        // Libero los nombres y borro los archivos solo después de mezclarlos
        for (size_t j = inicio; j < fin; j++)
        {
            remove(nombres_runs[j]);
            free(nombres_runs[j]);
        }
    }

    free(nombres_runs);

    // Llamada recursiva con los nuevos runs generados
    mezclar_runs(nuevos_nombres, nuevos_runs, a, nivel + 1);

    // Libero los nombres generados en este nivel (después de la recursión)
    for (size_t i = 0; i < nuevos_runs; i++)
    {
        free(nuevos_nombres[i]);
    }
    free(nuevos_nombres);
}


void mergesort_externo(const char *nombre_archivo, size_t M, size_t a)
{
    FILE *entrada = fopen(nombre_archivo, "rb");
    if (!entrada)
    {
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
}
