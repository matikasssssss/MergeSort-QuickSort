#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

void generar_datos_binarios(const char *nombre_archivo, size_t cant_elementos)
{
    FILE *f = fopen(nombre_archivo, "wb");
    if (!f)
    {
        perror("fopen");
        exit(1);
    }

    srand(time(NULL));
    for (size_t i = 0; i < cant_elementos; i++)
    {
        int64_t num = ((int64_t)rand() << 32) | rand(); // número pseudoaleatorio de 64 bits
        fwrite(&num, sizeof(int64_t), 1, f);
    }

    fclose(f);
}
