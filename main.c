#include "mergesort.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

    return 0;
}
