#ifndef DISKIO_H
#define DISKIO_H

#include <cstdint>
#include <fstream>

using namespace std;

/**
 * @brief Tamaño fijo de cada bloque de disco simulado, en bytes.
 */
#define BLOCK_SIZE 4096

/**
 * @brief Cantidad de elementos de tipo int64_t que caben en un bloque.
 */
#define ELEMENTS_PER_BLOCK (BLOCK_SIZE / sizeof(int64_t))

/**
 * @brief Lee una cantidad específica de elementos desde un bloque del archivo binario.
 *
 * Posiciona el puntero de lectura en el bloque indicado y lee `cantidad_elementos` del archivo hacia el buffer.
 * Incrementa `cont_lecturas` si se lee un bloque completo.
 *
 * @param f Archivo binario de entrada (`ifstream`) abierto en modo binario.
 * @param buffer Puntero al buffer donde se almacenarán los datos leídos.
 * @param bloque Índice del bloque desde el cual leer (posición = bloque * BLOCK_SIZE).
 * @param cantidad_elementos Cantidad de elementos `int64_t` a leer.
 */
void leer_bloque(ifstream &f, int64_t *buffer, size_t bloque, size_t cantidad_elementos);

/**
 * @brief Escribe una cantidad específica de elementos en un bloque del archivo binario.
 *
 * Si `bloque == (size_t)-1`, se escribe al final del archivo (modo concatenación).
 * Si se escribe un bloque completo, incrementa `cont_escrituras`.
 *
 * @param f Archivo binario de salida (`ofstream`) abierto en modo binario.
 * @param buffer Puntero al buffer que contiene los datos a escribir.
 * @param bloque Índice del bloque donde escribir (o `-1` para escribir al final).
 * @param cantidad_elementos Cantidad de elementos `int64_t` a escribir.
 */
void escribir_bloque(ofstream &f, const int64_t *buffer, size_t bloque, size_t cantidad_elementos);

/**
 * @brief Contador global de lecturas de bloques completas desde disco.
 */
extern size_t cont_lecturas;

/**
 * @brief Contador global de escrituras de bloques completas hacia disco.
 */
extern size_t cont_escrituras;

#endif // DISKIO_H
