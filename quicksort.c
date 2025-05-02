#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "diskio.h"

void intercambiar_elementos(FILE *f, size_t i, size_t j)
{
    int64_t a, b;

    // Leer elemento i
    size_t bloque_i = i / ELEMENTS_PER_BLOCK;
    size_t offset_i = i % ELEMENTS_PER_BLOCK;
    int64_t buffer_i[ELEMENTS_PER_BLOCK];
    leer_bloque(f, buffer_i, bloque_i);
    a = buffer_i[offset_i];

    // Leer elemento j
    size_t bloque_j = j / ELEMENTS_PER_BLOCK;
    size_t offset_j = j % ELEMENTS_PER_BLOCK;
    int64_t buffer_j[ELEMENTS_PER_BLOCK];
    leer_bloque(f, buffer_j, bloque_j);
    b = buffer_j[offset_j];

    // Intercambiar en memoria
    buffer_i[offset_i] = b;
    buffer_j[offset_j] = a;

    // Escribir cambios
    escribir_bloque(f, buffer_i, bloque_i);
    escribir_bloque(f, buffer_j, bloque_j);
}

int64_t leer_elemento(FILE *f, size_t pos)
{
    size_t bloque = pos / ELEMENTS_PER_BLOCK;
    size_t offset = pos % ELEMENTS_PER_BLOCK;
    int64_t buffer[ELEMENTS_PER_BLOCK];
    leer_bloque(f, buffer, bloque);
    return buffer[offset];
}

size_t particionar(FILE *f, size_t low, size_t high)
{
    int64_t pivot = leer_elemento(f, high);
    size_t i = low;

    for (size_t j = low; j < high; j++)
    {
        if (leer_elemento(f, j) < pivot)
        {
            intercambiar_elementos(f, i, j);
            i++;
        }
    }
    intercambiar_elementos(f, i, high);
    return i;
}

void quicksort_externo(FILE *f, size_t low, size_t high)
{
    if (low < high)
    {
        size_t pi = particionar(f, low, high);
        if (pi > 0) quicksort_externo(f, low, pi - 1); // evitar underflow
        quicksort_externo(f, pi + 1, high);
    }
}
