#ifndef UTILS_H
#define UTILS_H

#include <string>

/**
 * @brief Genera un archivo binario con números enteros aleatorios de 64 bits.
 *
 * Crea un archivo binario llamado `filename` que contiene números `uint64_t`
 * generados aleatoriamente. El tamaño del archivo será aproximadamente `size_in_mb` megabytes.
 *
 * @param filename Nombre del archivo binario a generar.
 * @param size_in_mb Tamaño del archivo deseado en megabytes.
 */
void generate_binary_file(const std::string &filename, size_t size_in_mb);

#endif // UTILS_H
