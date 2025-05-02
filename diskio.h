#ifndef DISKIO_H
#define DISKIO_H

#include <stdint.h>
#include <stdio.h>

#define BLOCK_SIZE 4096
#define ELEMENT_SIZE sizeof(int64_t)
#define ELEMENTS_PER_BLOCK (BLOCK_SIZE / ELEMENT_SIZE)

void leer_bloque(FILE *f, int64_t *buffer, size_t bloque);
void escribir_bloque(FILE *f, int64_t *buffer, size_t bloque);

extern size_t cont_lecturas;
extern size_t cont_escrituras;

#endif
