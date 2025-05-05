#include "diskio.h"
#include <fstream>
#include <iostream>

size_t cont_lecturas = 0;
size_t cont_escrituras = 0;

#define ELEMENT_SIZE sizeof(int64_t) // Asegúrate de que ELEMENT_SIZE esté definido correctamente

// Función para leer un bloque de datos desde el archivo
void leer_bloque(std::ifstream &f, int64_t *buffer, size_t bloque, size_t cantidad_elementos)
{
    f.seekg(bloque * BLOCK_SIZE, std::ios::beg);
    f.read(reinterpret_cast<char *>(buffer), ELEMENT_SIZE * cantidad_elementos);
    cont_lecturas++;
}

// Función para escribir un bloque de datos en el archivo
void escribir_bloque(std::ofstream &f, const int64_t *buffer, size_t bloque, size_t cantidad_elementos)
{
    f.seekp(bloque * BLOCK_SIZE, std::ios::beg);
    f.write(reinterpret_cast<const char *>(buffer), ELEMENT_SIZE * cantidad_elementos);
    cont_escrituras++;
}
