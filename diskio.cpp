#include "diskio.h"
#include <fstream>
#include <iostream>

size_t cont_lecturas = 0;
size_t cont_escrituras = 0;

#define BLOCK_SIZE 4096
#define ELEMENTS_PER_BLOCK (BLOCK_SIZE / sizeof(int64_t))

// Leer exactamente cantidad_elementos
void leer_bloque(std::ifstream &f, int64_t *buffer, size_t bloque, size_t cantidad_elementos)
{
    f.seekg(bloque * BLOCK_SIZE, std::ios::beg);
    f.read(reinterpret_cast<char *>(buffer), cantidad_elementos * sizeof(int64_t));
    cont_lecturas++;
}

// Escribir exactamente cantidad_elementos
void escribir_bloque(std::ofstream &f, const int64_t *buffer, size_t bloque, size_t cantidad_elementos)
{
    if (bloque != (size_t)-1)
        f.seekp(bloque * BLOCK_SIZE, std::ios::beg);
    else
        f.seekp(0, std::ios::end); // para concatenar sin sobrescribir

    f.write(reinterpret_cast<const char *>(buffer), cantidad_elementos * sizeof(int64_t));
    cont_escrituras++;
}
