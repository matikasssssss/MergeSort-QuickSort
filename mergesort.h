#ifndef MERGESORT_H
#define MERGESORT_H

#include <cstddef>
#include <string>

using namespace std;

/**
 * @brief Ejecuta el algoritmo de Mergesort Externo sobre un archivo binario de enteros de 64 bits.
 *
 * El algoritmo divide el archivo en runs iniciales que caben en memoria, los ordena y luego
 * aplica una mezcla jerárquica con aridad `a` hasta obtener el archivo completamente ordenado.
 * Este método es útil para ordenar datos que no caben completamente en memoria principal.
 *
 * @param nombre_archivo Nombre del archivo binario a ordenar. El archivo debe contener datos del tipo `int64_t`.
 * @param a Aridad de la mezcla. Define cuántos archivos se mezclan simultáneamente en cada paso del algoritmo.
 *
 * @post El archivo resultante se guarda como `"entrada_tmp.bin"`, y se eliminan los archivos temporales usados en el proceso.
 */
void mergesort_externo(const string &nombre_archivo, size_t a);

#endif // MERGESORT_H
