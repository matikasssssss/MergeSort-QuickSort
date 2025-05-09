#ifndef DISKIO_H
#define DISKIO_H

#include <cstdint>
#include <fstream>

#define BLOCK_SIZE 4096
#define ELEMENTS_PER_BLOCK (BLOCK_SIZE / sizeof(int64_t))

// Declaración de funciones para leer y escribir bloques en archivos
void leer_bloque(std::ifstream &f, int64_t *buffer, size_t bloque, size_t cantidad_elementos);
void escribir_bloque(std::ofstream &f, const int64_t *buffer, size_t bloque, size_t cantidad_elementos);

// Variables globales para contar las lecturas y escrituras
extern size_t cont_lecturas;
extern size_t cont_escrituras;

#endif
