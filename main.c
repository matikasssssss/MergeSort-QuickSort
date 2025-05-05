#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils.h"
#include "mergesort.h"
#include "diskio.h"

int main()
{
    size_t cantidad = 1000; // Por ejemplo, 10 mil elementos
    generar_datos_binarios("entrada.bin", cantidad);

    cont_lecturas = 0;
    cont_escrituras = 0;

    // Ejecutar el mergesort externo
    mergesort_externo("entrada.bin", BLOCK_SIZE, 8); // Por ejemplo, a = 8

    printf("Lecturas: %zu\n", cont_lecturas);
    printf("Escrituras: %zu\n", cont_escrituras);

    return 0;
}
