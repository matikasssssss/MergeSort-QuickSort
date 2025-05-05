#include "diskio.h"
#include <stdlib.h>

size_t cont_lecturas = 0;
size_t cont_escrituras = 0;

#define ELEMENT_SIZE sizeof(int64_t) // Asegúrate de que ELEMENT_SIZE esté definido correctamente

void leer_bloque(FILE *f, int64_t *buffer, size_t bloque, size_t cantidad_elementos)
{
    fseek(f, bloque * BLOCK_SIZE, SEEK_SET);
    fread(buffer, ELEMENT_SIZE, cantidad_elementos, f);
    cont_lecturas++;
}

void escribir_bloque(FILE *f, int64_t *buffer, size_t bloque, size_t cantidad_elementos)
{
    fseek(f, bloque * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, ELEMENT_SIZE, cantidad_elementos, f);
    cont_escrituras++;
}


