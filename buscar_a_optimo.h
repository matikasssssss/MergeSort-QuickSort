#ifndef BUSCAR_A_OPTIMO_H
#define BUSCAR_A_OPTIMO_H

#include <string>
#include <cstddef>

// Ejecuta mergesort_externo sobre una copia del archivo y devuelve la suma de lecturas + escrituras
size_t ejecutar_con_a(const std::string &archivo, size_t a);

// Realiza búsqueda ternaria sobre el valor de a que minimiza lecturas + escrituras
// B es el tamaño de bloque en bytes (BLOCK_SIZE), usado para limitar el rango superior de a
size_t buscar_a_optimo(const std::string &archivo, size_t B);

#endif // BUSCAR_A_OPTIMO_H
