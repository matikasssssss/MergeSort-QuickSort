#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils.h"
#include "mergesort.h"
#include "diskio.h"

void verificar_orden(const char *nombre_archivo)
{
    FILE *f = fopen(nombre_archivo, "rb");
    if (!f)
    {
        perror("fopen");
        return;
    }

    int64_t anterior, actual;
    if (fread(&anterior, sizeof(int64_t), 1, f) != 1)
    {
        printf("Archivo vacío o error de lectura.\n");
        fclose(f);
        return;
    }

    size_t pos = 1;
    while (fread(&actual, sizeof(int64_t), 1, f) == 1)
    {
        if (actual < anterior)
        {
            printf("ERROR: Datos no ordenados en la posición %zu: %ld > %ld\n", pos, anterior, actual);
            fclose(f);
            return;
        }
        anterior = actual;
        pos++;
    }

    fclose(f);
    printf("¡Archivo ordenado correctamente!\n");
}


int main()
{
    size_t cantidad = 1000000; // Por ejemplo, 10 mil elementos
    generar_datos_binarios("entrada.bin", cantidad);

    cont_lecturas = 0;
    cont_escrituras = 0;

    // Ejecutar el mergesort externo
    mergesort_externo("entrada.bin", BLOCK_SIZE, 8); // Por ejemplo, a = 8

    printf("Lecturas: %zu\n", cont_lecturas);
    printf("Escrituras: %zu\n", cont_escrituras);

    verificar_orden("entrada_tmp.bin");

    return 0;
}
