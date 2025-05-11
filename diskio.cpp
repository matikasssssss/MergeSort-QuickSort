#include "diskio.h"
#include <fstream>
#include <iostream>

using namespace std;

/**
 * @brief Tamaño fijo de bloque en bytes utilizado para simular accesos a disco.
 */
#define BLOCK_SIZE 4096

/**
 * @brief Cantidad de elementos de tipo int64_t que caben en un bloque de tamaño BLOCK_SIZE.
 */
#define ELEMENTS_PER_BLOCK (BLOCK_SIZE / sizeof(int64_t))

/**
 * @brief Contador global de lecturas completas desde disco.
 *
 * Se incrementa automáticamente en `leer_bloque()` cuando se lee un bloque completo de tamaño `ELEMENTS_PER_BLOCK`.
 */
size_t cont_lecturas = 0;

/**
 * @brief Contador global de escrituras completas a disco.
 *
 * Se incrementa automáticamente en `escribir_bloque()` cuando se escribe un bloque completo de tamaño `ELEMENTS_PER_BLOCK`.
 */
size_t cont_escrituras = 0;

/**
 * @brief Lee un bloque desde un archivo binario en una posición específica.
 *
 * Simula una lectura de disco accediendo exactamente `cantidad_elementos` elementos de tipo `int64_t`
 * desde el bloque especificado. Si se lee un bloque completo (`ELEMENTS_PER_BLOCK`), se incrementa
 * el contador global `cont_lecturas`.
 *
 * @param f Referencia al archivo binario de entrada abierto en modo lectura (`ifstream`).
 * @param buffer Puntero al buffer donde se almacenarán los datos leídos.
 * @param bloque Índice del bloque desde donde se realizará la lectura (bloque 0 corresponde al inicio del archivo).
 * @param cantidad_elementos Cantidad de elementos `int64_t` a leer desde el bloque.
 *
 * @note Si `cantidad_elementos` es menor que `ELEMENTS_PER_BLOCK`, no se cuenta como una lectura completa.
 */
void leer_bloque(ifstream &f, int64_t *buffer, size_t bloque, size_t cantidad_elementos)
{
    f.seekg(bloque * BLOCK_SIZE, ios::beg);
    f.read(reinterpret_cast<char *>(buffer), cantidad_elementos * sizeof(int64_t));
    if (cantidad_elementos == ELEMENTS_PER_BLOCK)
        cont_lecturas++;
}

/**
 * @brief Escribe un bloque en un archivo binario en una posición específica.
 *
 * Simula una escritura en disco escribiendo `cantidad_elementos` desde el buffer al archivo.
 * Si se escribe un bloque completo (`ELEMENTS_PER_BLOCK`), se incrementa el contador global `cont_escrituras`.
 *
 * @param f Referencia al archivo binario de salida abierto en modo escritura (`ofstream`).
 * @param buffer Puntero al buffer que contiene los datos a escribir.
 * @param bloque Índice del bloque donde se escribirá. Si es `-1` (cast a `size_t`), se escribe al final del archivo (modo concatenación).
 * @param cantidad_elementos Cantidad de elementos `int64_t` a escribir.
 *
 * @note Se usa `seekp` para posicionarse antes de escribir. Para bloque `-1`, se posiciona al final (`ios::end`).
 */
void escribir_bloque(ofstream &f, const int64_t *buffer, size_t bloque, size_t cantidad_elementos)
{
    if (bloque != (size_t)-1)
        f.seekp(bloque * BLOCK_SIZE, ios::beg);
    else
        f.seekp(0, ios::end); // para concatenar sin sobrescribir

    f.write(reinterpret_cast<const char *>(buffer), cantidad_elementos * sizeof(int64_t));
    if (cantidad_elementos == ELEMENTS_PER_BLOCK)
        cont_escrituras++;
}
