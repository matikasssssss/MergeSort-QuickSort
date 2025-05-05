#include "mergesort.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

int verificar_orden(const char *archivo)
{
    printf("Verificando orden del archivo: %s\n", archivo);
    FILE *f = fopen(archivo, "rb");
    if (!f)
    {
        perror("fopen");
        return 0;
    }

    int64_t anterior, actual;
    if (fread(&anterior, sizeof(int64_t), 1, f) != 1)
    {
        fclose(f);
        return 1; // Archivo vacío o con un solo elemento
    }


    while (fread(&actual, sizeof(int64_t), 1, f) == 1)
    {
        if (actual < anterior)
        {
            fclose(f);
            return 0; // No está ordenado
        }
        anterior = actual;
    }

    fclose(f);
    return 1; // Orden correcto
}

int main()
{
    generar_datos_binarios("entrada.bin", 10000000); // Genera datos aleatorios
    mergesort_externo("entrada.bin", 1024, 4);       // Ejecuta el mergesort

    printf("Mergesort finalizado.\n");

    if (verificar_orden("entrada_tmp.bin"))
    {
        printf("Mergesort finalizado correctamente. El archivo está ordenado.\n");
    }
    else
    {
        printf("Error: El archivo no está ordenado.\n");
    }

    verificar_orden("entrada_tmp.bin");

    return 0;
}
