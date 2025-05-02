#include "diskio.h"
#include <stdlib.h>

size_t contador_lecturas = 0;
size_t contador_escrituras = 0;

void leer_bloque(FILE *f, int64_t *buffer, size_t bloque)
{
    fseek(f, bloque * BLOCK_SIZE, SEEK_SET);
    fread(buffer, ELEMENT_SIZE, ELEMENTS_PER_BLOCK, f);
    contador_lecturas++;
}

void escribir_bloque(FILE *f, int64_t *buffer, size_t bloque)
{
    fseek(f, bloque * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, ELEMENT_SIZE, ELEMENTS_PER_BLOCK, f);
    contador_escrituras++;
}
