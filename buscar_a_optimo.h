#ifndef BUSCAR_A_OPTIMO_H
#define BUSCAR_A_OPTIMO_H

#include <string>
#include <cstddef>

using namespace std;

/**
 * @brief Ejecuta Mergesort Externo sobre una copia del archivo con una aridad dada.
 *
 * La función realiza una copia del archivo original a un archivo temporal,
 * ejecuta `mergesort_externo()` con el valor de aridad `a` y devuelve la
 * cantidad total de accesos a disco (lecturas + escrituras).
 *
 * @param archivo Nombre del archivo binario original (no se modifica).
 * @param a Valor de aridad a utilizar en el algoritmo.
 *
 * @return Cantidad total de accesos a disco realizados durante la ejecución.
 *         Devuelve `std::numeric_limits<size_t>::max()` en caso de error de lectura o escritura.
 */
size_t ejecutar_con_a(const string &archivo, size_t a);

/**
 * @brief Busca el valor óptimo de aridad `a` que minimiza la suma de lecturas y escrituras al disco.
 *
 * Realiza una búsqueda ternaria sobre el rango [2, 512] (o definido por `B`) para encontrar
 * la aridad que produce el menor número de I/Os durante la ejecución de `mergesort_externo()`.
 *
 * @param archivo Nombre del archivo binario sobre el cual se realiza la prueba.
 * @param B Tamaño del bloque en bytes. Se usa para estimar el límite superior de la aridad.
 *
 * @return Valor óptimo de `a` que minimiza el número total de accesos a disco.
 */
size_t buscar_a_optimo(const string &archivo, size_t B);

#endif // BUSCAR_A_OPTIMO_H
