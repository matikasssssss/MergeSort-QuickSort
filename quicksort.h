#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <string>
#include <cstddef>

/**
 * @brief Ordena un archivo binario de enteros de 64 bits usando Quicksort externo.
 *
 * Este algoritmo particiona el archivo en subarreglos según pivotes elegidos desde un bloque aleatorio.
 * Cada subarreglo es ordenado recursivamente, y luego se concatenan todos los resultados.
 *
 * @param archivo Nombre del archivo a ordenar (se sobrescribe al final).
 * @param M Tamaño de memoria principal disponible (en bytes).
 * @param a Aridad: cantidad de particiones a realizar en cada paso recursivo.
 * @param nivel Nivel actual de recursión (por defecto es 0, útil para generar nombres únicos de archivos temporales).
 */
void quicksort_externo(const std::string &archivo, size_t M, size_t a, int nivel = 0);

#endif // QUICKSORT_H