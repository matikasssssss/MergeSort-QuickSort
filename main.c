#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils.h"
#include "mergesort.h"
#include "diskio.h"

#define NUM_ELEMENTOS 10000
#define M BLOCK_SIZE
#define A 4 // Prueba con un a fijo

int main()
{
    generar_datos_binarios("entrada.bin", NUM_ELEMENTOS);

    mergesort_externo("entrada.bin", M, A);

    FILE *f = fopen("entrada_tmp.bin", "rb");
    if (!f)
    {
        perror("fopen");
        return 1;
    }

    int64_t anterior, actual;
    if (fread(&anterior, sizeof(int64_t), 1, f) != 1)
    {
        printf("Archivo vacío o error de lectura.\n");
        fclose(f);
        return 1;
    }

    size_t pos = 1;
    while (fread(&actual, sizeof(int64_t), 1, f) == 1)
    {
        if (actual < anterior)
        {
            printf("ERROR: Datos no ordenados en la posición %zu: %ld > %ld\n", pos, anterior, actual);
            fclose(f);
            return 1;
        }
        anterior = actual;
        pos++;
    }

    fclose(f);
    printf("¡Archivo ordenado correctamente!\n");
    return 0;
}
